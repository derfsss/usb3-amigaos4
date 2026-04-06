
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"
#include "XHCI.h"

// --

SEC_CODE S32 XHCI_Interrupt_Build( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct RealFunctionNode *fn;
struct USB2_EndPointNode *ep;
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_Ring *ring;
struct XHCI_TRB trb;
S32 handled;
U32 slotid;
U32 dci;
U32 ring_idx;
U32 data_len;
U32 pid;
PTR bounce;
U32 bounce_phy;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Interrupt_Build" );

	fn   = ioreq->req_Function;
	ep   = ioreq->req_EndPoint;
	xhci = & hn->hn_HCD.XHCI;

	handled = TRUE;

	// Look up slot
	slotid = xhci->SlotID_ByAddress[ fn->fkt_Address ];

	if ( ! slotid )
	{
		USBERROR( "XHCI: Interrupt_Build: no slot for address %ld", (U32) fn->fkt_Address );
		ioreq->req_Public.io_Error = USB2Err_Stack_FunctionNotFound;
		goto bailout;
	}

	slot = xhci->Slots[ slotid ];

	if ( ! slot )
	{
		USBERROR( "XHCI: Interrupt_Build: slot %ld not allocated", slotid );
		ioreq->req_Public.io_Error = USB2Err_Stack_FunctionNotFound;
		goto bailout;
	}

	// Compute DCI and get transfer ring
	dci      = XHCI_Endpoint_DCI( ep->ep_Number, ep->ep_Direction );
	ring_idx = dci - 1;

	if ( ring_idx >= 31 || ! slot->transfer_ring[ ring_idx ].trbs )
	{
		USBERROR( "XHCI: Interrupt_Build: no ring for DCI %ld (ep %ld dir %ld)",
			dci, ep->ep_Number, ep->ep_Direction );
		ioreq->req_Public.io_Error = USB2Err_Stack_EndPointNotFound;
		goto bailout;
	}

	ring = & slot->transfer_ring[ ring_idx ];

	// Initialize IORequest XHCI state
	MEM_SET( & ioreq->req_HCD.XHCI, 0, sizeof( struct __xhci ) );
	ioreq->req_HCD.XHCI.SlotID = slotid;
	ioreq->req_HCD.XHCI.DCI    = dci;

	data_len = ioreq->req_Public.io_Length;

	// Allocate DMA bounce buffer
	bounce = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! bounce )
	{
		USBERROR( "XHCI: Interrupt_Build: error allocating DMA buffer" );
		ioreq->req_Public.io_Error = USB2Err_Stack_NoMemory;
		goto bailout;
	}

	bounce_phy = ((struct Mem_FreeNode *) bounce)->mfn_Addr;

	ioreq->req_HCD.XHCI.DataBuffer    = bounce;
	ioreq->req_HCD.XHCI.DataBufferPhy = bounce_phy;
	ioreq->req_HCD.XHCI.DataBufferLen = data_len;

	// For OUT transfers, copy data to bounce buffer
	if (( ioreq->req_Public.io_Command != CMD_READ ) && ( ioreq->req_Public.io_Data ))
	{
		MEM_COPY( ioreq->req_Public.io_Data, bounce, data_len );
	}
	else
	{
		MEM_SET( bounce, 0, data_len );
	}

	// Build Normal TRB
	MEM_SET( & trb, 0, sizeof( trb ) );

	trb.trb_param_lo = LE_SWAP32( bounce_phy );
	trb.trb_param_hi = 0;
	trb.trb_status   = LE_SWAP32( XHCI_TRB_SET_XFERLEN( data_len ) | XHCI_TRB_SET_TDSIZE( 0 ) | XHCI_TRB_SET_INTR( 0 ) );
	trb.trb_control  = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_NORMAL ) | XHCI_TRB_IOC | XHCI_TRB_ISP );

	XHCI_Ring_Enqueue_TRB( hn, ring, & trb );

	handled = FALSE;

bailout:

	TASK_NAME_LEAVE();

	return( handled );
}

// --
