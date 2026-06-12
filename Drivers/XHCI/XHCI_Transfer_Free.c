
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
// Cancel a transfer the hardware still owns: stop the endpoint, then
// move its ring dequeue pointer past everything that was enqueued so
// the TD is flushed, and ring the doorbell so any later TDs run.
// Called from Bulk_Remove/Interrupt_Remove for IORequests that are
// being removed (AbortIO, timeout, detach) before their Transfer Event
// arrived. No-op for transfers that already completed.

SEC_CODE void XHCI_Transfer_Abort( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_Ring *ring;
U32 slotid;
U32 dci;
U32 deq;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci   = & hn->hn_HCD.XHCI;
	slotid = ioreq->req_HCD.XHCI.SlotID;
	dci    = ioreq->req_HCD.XHCI.DCI;

	if (( ioreq->req_HCD.XHCI.Completed )
	||	( ! slotid ) || ( slotid > xhci->MaxSlots )
	||	( dci < 1 ) || ( dci > 31 ))
	{
		return;
	}

	slot = xhci->Slots[ slotid ];

	if (( ! slot ) || ( ! slot->transfer_ring[ dci - 1 ].trbs ))
	{
		return;
	}

	// Dead controller: nothing to cancel at the hardware level
	if ( PCI_READLONG( xhci->OpBase + XHCI_USBSTS ) & XHCI_STS_HCH )
	{
		return;
	}

	ring = & slot->transfer_ring[ dci - 1 ];

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Transfer_Abort: slot=%ld dci=%ld -- stopping endpoint and flushing ring\n",
		slotid, dci );

	XHCI_Cmd_StopEndpoint( hn, slotid, dci );

	// Flush: dequeue := current enqueue position (skips the whole TD),
	// DCS = the producer cycle state at that position
	deq = ( ring->phys + ( ring->enqueue * XHCI_TRB_SIZE ) )
		| ( ring->cycle ? 1 : 0 );

	XHCI_Cmd_SetTRDequeue( hn, slotid, dci, deq );

	// Nothing of this transfer reached the device as far as we know
	ioreq->req_HCD.XHCI.Residual = ioreq->req_HCD.XHCI.DataBufferLen;

	// Restart the endpoint for any TDs enqueued after the flush point
	PCI_WRITELONG( xhci->DoorbellBase + 4 * slotid, dci );
}

// --

SEC_CODE void XHCI_Transfer_Free( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
	struct USBBase *usbbase = hn->hn_USBBase;

	// Single bounce buffer (control / interrupt). DataBufferPool records
	// which pool it came from -- control uses MEMID_HCD_20k, interrupt
	// MEMID_HCD_4k. (Freeing into the wrong pool poisons that pool's
	// free list with wrong-sized chunks.)

	if ( ioreq->req_HCD.XHCI.DataBuffer )
	{
		MEMORY_FREE( ioreq->req_HCD.XHCI.DataBufferPool,
			ioreq->req_HCD.XHCI.DataBuffer, ioreq->req_HCD.XHCI.DataBufferPhy );
		ioreq->req_HCD.XHCI.DataBuffer = NULL;
	}

	// Bulk chunk list + chunk table

	if ( ioreq->req_HCD.XHCI.BounceTable )
	{
		struct XHCI_BounceChunk *chunks = ioreq->req_HCD.XHCI.BounceTable;
		U32 cnt;

		for ( cnt = 0 ; cnt < ioreq->req_HCD.XHCI.BounceCnt ; cnt++ )
		{
			if ( chunks[ cnt ].Virt )
			{
				MEMORY_FREE( MEMID_HCD_20k, chunks[ cnt ].Virt, chunks[ cnt ].Phy );
			}
		}

		MEMORY_FREE( MEMID_HCD_4k, chunks, ioreq->req_HCD.XHCI.BounceTablePhy );

		ioreq->req_HCD.XHCI.BounceTable = NULL;
		ioreq->req_HCD.XHCI.BounceCnt   = 0;
	}
}

// --
