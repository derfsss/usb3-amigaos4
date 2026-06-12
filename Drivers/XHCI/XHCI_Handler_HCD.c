
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "XHCI.h"

// --
// Drain deferred per-slot work queued by Function_Detach and
// Transfer_Abort from non-HCD contexts: endpoint stop+flush requests
// and slot disposal. Only ever entered from the HCD task; DrainBusy
// guards against recursion (the command waits below call back into
// XHCI_Handler_HCD to poll the event ring).

SEC_CODE static void XHCI_Drain_Deferred( struct USB3_HCDNode *hn )
{
struct XHCI_Slot *slot;
struct XHCI_Ring *ring;
struct _XHCI *xhci;
U32 slotid;
U32 stop;
U32 dci;
U32 deq;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;

	xhci->DrainBusy = 1;

	for ( slotid = 1 ; slotid <= xhci->MaxSlots ; slotid++ )
	{
		slot = xhci->Slots[ slotid ];

		if ( ! slot )
		{
			continue;
		}

		stop = slot->PendingStopDCI;

		if (( stop ) && ( ! slot->PendingDisable ))
		{
			// Stop + flush individual endpoints (skipped when the whole
			// slot is going away below)
			slot->PendingStopDCI = 0;

			for ( dci = 1 ; dci <= 31 ; dci++ )
			{
				if (( ! ( stop & ( 1UL << dci ) )) || ( ! slot->transfer_ring[ dci - 1 ].trbs ))
				{
					continue;
				}

				usbbase->usb_IExec->DebugPrintF(
					"USB3: Drain: stop+flush slot=%ld dci=%ld\n", slotid, dci );

				ring = & slot->transfer_ring[ dci - 1 ];

				XHCI_Cmd_StopEndpoint( hn, slotid, dci );

				deq = ( ring->phys + ( ring->enqueue * XHCI_TRB_SIZE ) )
					| ( ring->cycle ? 1 : 0 );

				XHCI_Cmd_SetTRDequeue( hn, slotid, dci, deq );

				PCI_WRITELONG( xhci->DoorbellBase + 4 * slotid, dci );
			}
		}

		if ( slot->PendingDisable )
		{
			usbbase->usb_IExec->DebugPrintF(
				"USB3: Drain: disabling slot %ld\n", slotid );

			slot->PendingDisable = 0;

			if ( ! ( PCI_READLONG( xhci->OpBase + XHCI_USBSTS ) & XHCI_STS_HCH ) )
			{
				XHCI_Cmd_DisableSlot( hn, slotid );
			}

			XHCI_Slot_Free( hn, slotid );
		}
	}

	xhci->DrainBusy = 0;
}

// --

// Anything queued, and is this a context that may process it?

SEC_CODE static S32 XHCI_Drain_Wanted( struct USB3_HCDNode *hn, struct USBBase *usbbase )
{
struct XHCI_Slot *slot;
struct _XHCI *xhci;
U32 slotid;

	xhci = & hn->hn_HCD.XHCI;

	if (( xhci->DrainBusy )
	||	( ! xhci->Slots )
	||	( usbbase->usb_IExec->FindTask( NULL ) != xhci->HCDTask ))
	{
		return( FALSE );
	}

	for ( slotid = 1 ; slotid <= xhci->MaxSlots ; slotid++ )
	{
		slot = xhci->Slots[ slotid ];

		if (( slot ) && (( slot->PendingDisable ) || ( slot->PendingStopDCI )))
		{
			return( TRUE );
		}
	}

	return( FALSE );
}

// --

SEC_CODE void XHCI_Handler_HCD( struct USB3_HCDNode *hn, U32 mask UNUSED )
{
struct _XHCI *xhci;
struct XHCI_TRB *trb;
struct RealFunctionNode *fn;
struct RealRequest *ioreq;
struct RealRequest *next;
U32 idx;
U32 type;
U32 ctrl;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Handler_HCD" );

	xhci = & hn->hn_HCD.XHCI;

	// -- Process Event Ring

	while( 1 )
	{
		idx = xhci->EvtRing.dequeue;
		trb = & xhci->EvtRing.trbs[ idx ];

		ctrl = LE_SWAP32( trb->trb_control );

		// Check cycle bit -- if it doesn't match our expected cycle, ring is empty
		if ( ( ctrl & XHCI_TRB_CYCLE ) != xhci->EvtRing.cycle )
		{
			break;
		}

		type = XHCI_TRB_GET_TYPE( ctrl );

		/**/ if ( type == XHCI_TRB_CMD_COMPLETION )
		{
			// Command Completion Event
			xhci->CmdResult_Code   = XHCI_TRB_GET_COMPCODE( LE_SWAP32( trb->trb_status ) );
			xhci->CmdResult_SlotID = XHCI_TRB_GET_SLOT( ctrl );
			xhci->CmdResult_Param  = LE_SWAP32( trb->trb_param_lo );

			// Signal command waiter via the signal mask
			// (command waiter is in the same task context, so use TASK_SETSIGNAL or direct signal)
			usbbase->usb_IExec->Signal( usbbase->usb_IExec->FindTask(NULL), xhci->Signal_Command.sig_Signal_Mask );
		}
		else if ( type == XHCI_TRB_PORT_STATUS_CHG )
		{
			// Port Status Change Event
			usbbase->usb_IExec->DebugPrintF( "XHCI: Port status change event\n" );
		}
		else if ( type == XHCI_TRB_TRANSFER_EVENT )
		{
			// Transfer Event -- match to pending IORequest by slot + DCI
			U32 evt_slot = XHCI_TRB_GET_SLOT( ctrl );
			U32 evt_dci  = XHCI_TRB_GET_EP( ctrl );
			U32 evt_ed   = ( ctrl & XHCI_TRB_EVENT_ED ) ? 1 : 0;
			U32 evt_cc   = XHCI_TRB_GET_COMPCODE( LE_SWAP32( trb->trb_status ) );
			U32 evt_len  = XHCI_TRB_GET_XFERLEN( LE_SWAP32( trb->trb_status ) );

			usbbase->usb_IExec->DebugPrintF( "XHCI: TransferEvent slot=%ld dci=%ld cc=%ld len=%ld ed=%ld\n",
				evt_slot, evt_dci, evt_cc, evt_len, evt_ed );

			// Stash the last transfer outcome on the slot itself so that
			// in-context callers (e.g. the GET_DESCRIPTOR(8) injection in
			// Port_Set_Reset that runs without an IORequest) can poll for
			// completion. Existing IORequest matching below still applies.
			if ( evt_slot && evt_slot <= xhci->MaxSlots && xhci->Slots[ evt_slot ] )
			{
				xhci->Slots[ evt_slot ]->LastTransfer_CC   = (U8) evt_cc;
				xhci->Slots[ evt_slot ]->LastTransfer_Done = 1;
			}

			struct RealRequest *scan = hn->hn_Active_Transfer_List.uh_Head;

			while( scan )
			{
				if (( scan->req_HCD.XHCI.SlotID == evt_slot )
				&&	( scan->req_HCD.XHCI.DCI == evt_dci )
				&&	( ! scan->req_HCD.XHCI.Completed ))
				{
					scan->req_HCD.XHCI.Completed      = 1;
					scan->req_HCD.XHCI.CompletionCode = evt_cc;

					if ( scan->req_HCD.XHCI.EventData )
					{
						// Event Data TRB completion: evt_len is the EDTLA,
						// i.e. total bytes TRANSFERRED for the TD
						U32 total = scan->req_HCD.XHCI.DataBufferLen;

						scan->req_HCD.XHCI.Residual =
							( evt_len <= total ) ? ( total - evt_len ) : 0;
					}
					else
					{
						// Plain TRB completion: evt_len is the residual
						scan->req_HCD.XHCI.Residual = evt_len;
					}

					if ( evt_cc != XHCI_CC_SUCCESS && evt_cc != XHCI_CC_SHORT_PACKET )
					{
						switch( evt_cc )
						{
							case XHCI_CC_STALL:
								scan->req_Public.io_Error = USB3Err_Host_Stall;
								break;
							case XHCI_CC_BABBLE:
								scan->req_Public.io_Error = USB3Err_Host_Overflow;
								break;
							case XHCI_CC_USB_TRANSACTION:
								scan->req_Public.io_Error = USB3Err_Host_Timeout;
								break;
							default:
								scan->req_Public.io_Error = USB3Err_Host_UnknownError;
								break;
						}
					}

					break;
				}

				scan = Node_Next( scan );
			}
		}

		// Advance dequeue index
		idx++;

		if ( idx >= xhci->EvtRing.size )
		{
			idx = 0;
			xhci->EvtRing.cycle ^= 1;
		}

		xhci->EvtRing.dequeue = idx;
	}

	// -- Update Event Ring Dequeue Pointer

	{
		U32 erdp = xhci->EvtRing.phys + ( xhci->EvtRing.dequeue * XHCI_TRB_SIZE );

		PCI_WRITELONG( xhci->RunBase + XHCI_ERDP_LO(0), erdp | XHCI_ERDP_EHB );
		PCI_WRITELONG( xhci->RunBase + XHCI_ERDP_HI(0), 0 );
	}

	// -- Deferred endpoint stops / slot disposal (HCD task only)

	if ( XHCI_Drain_Wanted( hn, usbbase ) )
	{
		XHCI_Drain_Deferred( hn );
	}

	// -- Check active transfers (same pattern as EHCI_Handler_HCD)

	ioreq = hn->hn_Active_Transfer_List.uh_Head;

	while( ioreq )
	{
		next = Node_Next( ioreq );
		fn = ioreq->req_Function;

		SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

		HCD_TRANSFER_CHECK( hn, ioreq, FALSE );

		SEMAPHORE_RELEASE( & fn->fkt_Semaphore );

		ioreq = next;
	}

	TASK_NAME_LEAVE();
}

// --
