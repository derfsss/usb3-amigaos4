
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
// Build a bulk transfer as ONE xHCI TD:
//
//   Normal TRB (CHAIN) -> ... -> Normal TRB (CHAIN) -> Event Data TRB (IOC)
//
// Each Normal TRB carries up to one MEMID_HCD_20k bounce chunk (20480
// bytes -- an exact multiple of the 512-byte HS max packet, so chunk
// boundaries never split a USB packet). The chunk table itself lives in
// a MEMID_HCD_4k allocation recorded on the IORequest so Bulk_Length can
// copy IN data back and Transfer_Free can release everything.
//
// The closing Event Data TRB is what makes short packets work without
// per-TRB bookkeeping: ISP is NOT set on the data TRBs, so a short packet
// generates no event of its own -- the controller skips to the end of the
// TD and the Event Data TRB then yields a single Transfer Event whose
// length field is the EDTLA, i.e. the TOTAL bytes actually transferred
// (CC = Short Packet). Handler_HCD converts that to a residual using the
// EventData flag set here.

#define XHCI_BULK_CHUNK			20480

// Chunk count is limited by whichever is smaller: the 4 KB chunk table
// (341 entries) or the transfer ring -- one TD must fit in the ring in
// one piece (256 slots minus the Link TRB, the closing Event Data TRB
// and a possible ZLP TRB). Ring wins: 252 chunks = ~5 MB per IORequest.
#define XHCI_BULK_TABLE_CHUNKS	( 4096 / sizeof( struct XHCI_BounceChunk ) )
#define XHCI_BULK_RING_CHUNKS	( XHCI_TRB_RING_SIZE - 4 )
#define XHCI_BULK_MAX_CHUNKS	MIN( XHCI_BULK_TABLE_CHUNKS, XHCI_BULK_RING_CHUNKS )

SEC_CODE S32 XHCI_Bulk_Build( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct XHCI_BounceChunk *chunks;
struct USB3_EndPointNode *ep;
struct RealFunctionNode *fn;
struct XHCI_Slot *slot;
struct XHCI_Ring *ring;
struct XHCI_TRB trb;
struct _XHCI *xhci;
S32 handled;
S32 zero;
U32 slotid;
U32 dci;
U32 ring_idx;
U32 total;
U32 remain;
U32 maxpkt;
U32 ntrbs;
U32 cnt;
U8 *src;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Bulk_Build" );

	fn   = ioreq->req_Function;
	ep   = ioreq->req_EndPoint;
	xhci = & hn->hn_HCD.XHCI;

	handled = TRUE;

	// Look up slot
	slotid = xhci->SlotID_ByAddress[ fn->fkt_Address ];

	if ( ! slotid )
	{
		USBERROR( "XHCI: Bulk_Build: no slot for address %ld", (U32) fn->fkt_Address );
		ioreq->req_Public.io_Error = USB3Err_Stack_FunctionNotFound;
		goto bailout;
	}

	slot = xhci->Slots[ slotid ];

	if ( ! slot )
	{
		USBERROR( "XHCI: Bulk_Build: slot %ld not allocated", slotid );
		ioreq->req_Public.io_Error = USB3Err_Stack_FunctionNotFound;
		goto bailout;
	}

	// Compute DCI and get transfer ring
	dci      = XHCI_Endpoint_DCI( ep->ep_Number, ep->ep_Direction );
	ring_idx = dci - 1;

	if ( ring_idx >= 31 || ! slot->transfer_ring[ ring_idx ].trbs )
	{
		USBERROR( "XHCI: Bulk_Build: no ring for DCI %ld (ep %ld dir %ld)",
			dci, ep->ep_Number, ep->ep_Direction );
		ioreq->req_Public.io_Error = USB3Err_Stack_EndPointNotFound;
		goto bailout;
	}

	ring = & slot->transfer_ring[ ring_idx ];

	// Initialize IORequest XHCI state. Transfer_Free cleans up whatever
	// is recorded here, including on the error paths below.
	MEM_SET( & ioreq->req_HCD.XHCI, 0, sizeof( struct __xhci ) );
	ioreq->req_HCD.XHCI.SlotID = slotid;
	ioreq->req_HCD.XHCI.DCI    = dci;

	total  = ioreq->req_Public.io_Length;
	maxpkt = ep->ep_MaxPacketSize ? ep->ep_MaxPacketSize : 512;

	ntrbs = ( total + XHCI_BULK_CHUNK - 1 ) / XHCI_BULK_CHUNK;

	if ( ntrbs > XHCI_BULK_MAX_CHUNKS )
	{
		USBERROR( "XHCI: Bulk_Build: io_Length %ld exceeds %ld chunk limit",
			total, (U32) XHCI_BULK_MAX_CHUNKS );
		ioreq->req_Public.io_Error = USB3Err_Stack_NoMemory;
		goto bailout;
	}

	// Explicit zero-length packet termination for OUT transfers, same
	// rule as the EHCI driver
	if (( ioreq->req_Public.io_Command != CMD_READ )
	&&	( ioreq->req_Public.io_AddZeroPackets )
	&&	( total != 0 )
	&&	(( total % maxpkt ) == 0 ))
	{
		zero = TRUE;
	}
	else
	{
		zero = FALSE;
	}

	// Allocate the chunk table (also used for zero-payload transfers,
	// where it stays empty -- BounceCnt 0)
	chunks = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! chunks )
	{
		USBERROR( "XHCI: Bulk_Build: error allocating chunk table" );
		ioreq->req_Public.io_Error = USB3Err_Stack_NoMemory;
		goto bailout;
	}

	ioreq->req_HCD.XHCI.BounceTable    = chunks;
	ioreq->req_HCD.XHCI.BounceTablePhy = ((struct Mem_FreeNode *) chunks)->mfn_Addr;
	ioreq->req_HCD.XHCI.DataBufferLen  = total;

	// Allocate bounce chunks; copy payload for OUT transfers
	src    = ioreq->req_Public.io_Data;
	remain = total;

	for ( cnt = 0 ; cnt < ntrbs ; cnt++ )
	{
		U32 l = MIN( XHCI_BULK_CHUNK, remain );
		PTR buf = MEMORY_ALLOC( MEMID_HCD_20k, FALSE );

		if ( ! buf )
		{
			USBERROR( "XHCI: Bulk_Build: error allocating bounce chunk %ld/%ld", cnt, ntrbs );
			ioreq->req_Public.io_Error = USB3Err_Stack_NoMemory;
			goto bailout;
		}

		chunks[ cnt ].Virt = buf;
		chunks[ cnt ].Phy  = ((struct Mem_FreeNode *) buf)->mfn_Addr;
		chunks[ cnt ].Len  = l;

		ioreq->req_HCD.XHCI.BounceCnt = cnt + 1;

		if (( ioreq->req_Public.io_Command != CMD_READ ) && ( src ))
		{
			MEM_COPY( src, buf, l );
			src += l;
		}

		remain -= l;
	}

	// -- Enqueue the data TRBs (all CHAINed; no IOC/ISP -- the Event
	// Data TRB at the end reports for the whole TD)

	remain = total;

	for ( cnt = 0 ; cnt < ntrbs ; cnt++ )
	{
		U32 l = chunks[ cnt ].Len;
		U32 tdsize;

		remain -= l;

		// TD Size: number of max-packets left in the TD after this TRB,
		// capped at 31; must be 0 on the last data TRB (xHCI 4.11.2.4)
		tdsize = ( remain + maxpkt - 1 ) / maxpkt;
		if ( tdsize > 31 )
		{
			tdsize = 31;
		}

		MEM_SET( & trb, 0, sizeof( trb ) );
		trb.trb_param_lo = LE_SWAP32( chunks[ cnt ].Phy );
		trb.trb_param_hi = 0;
		trb.trb_status   = LE_SWAP32(
			XHCI_TRB_SET_XFERLEN( l ) |
			XHCI_TRB_SET_TDSIZE( tdsize ) |
			XHCI_TRB_SET_INTR( 0 ) );
		trb.trb_control  = LE_SWAP32(
			XHCI_TRB_SET_TYPE( XHCI_TRB_NORMAL ) |
			XHCI_TRB_CHAIN );

		XHCI_Ring_Enqueue_TRB( hn, ring, & trb );
	}

	// Zero-payload transfer still needs one (zero-length) data TRB so
	// the TD exists on the wire
	if ( total == 0 )
	{
		MEM_SET( & trb, 0, sizeof( trb ) );
		trb.trb_control = LE_SWAP32(
			XHCI_TRB_SET_TYPE( XHCI_TRB_NORMAL ) |
			XHCI_TRB_CHAIN );

		XHCI_Ring_Enqueue_TRB( hn, ring, & trb );
	}

	// Explicit ZLP terminator (OUT, exact multiple of maxpkt)
	if ( zero )
	{
		MEM_SET( & trb, 0, sizeof( trb ) );
		trb.trb_control = LE_SWAP32(
			XHCI_TRB_SET_TYPE( XHCI_TRB_NORMAL ) |
			XHCI_TRB_CHAIN );

		XHCI_Ring_Enqueue_TRB( hn, ring, & trb );
	}

	// -- Closing Event Data TRB: IOC, param carries the IORequest pointer
	// (echoed back in the Transfer Event for diagnostics)

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_param_lo = LE_SWAP32( (U32) ioreq );
	trb.trb_param_hi = 0;
	trb.trb_control  = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_EVENT_DATA ) |
		XHCI_TRB_IOC );

	XHCI_Ring_Enqueue_TRB( hn, ring, & trb );

	ioreq->req_HCD.XHCI.EventData = 1;

	handled = FALSE;

bailout:

	TASK_NAME_LEAVE();

	return( handled );
}

// --
