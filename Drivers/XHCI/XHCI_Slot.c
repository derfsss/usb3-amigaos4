
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

// Initialize a transfer ring (TRB array with Link TRB at end)

static S32 XHCI_Ring_Init( struct USBBase *usbbase, struct XHCI_Ring *ring )
{
struct XHCI_TRB *link;

	ring->trbs = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! ring->trbs )
	{
		return( FALSE );
	}

	ring->phys    = ((struct Mem_FreeNode *) ring->trbs)->mfn_Addr;
	ring->size    = 4096 / XHCI_TRB_SIZE;	// 256 TRBs
	ring->enqueue = 0;
	ring->dequeue = 0;
	ring->cycle   = 1;

	MEM_SET( ring->trbs, 0, 4096 );

	// Place Link TRB at last position
	link = & ring->trbs[ ring->size - 1 ];
	link->trb_param_lo = LE_SWAP32( ring->phys );
	link->trb_param_hi = 0;
	link->trb_status   = 0;
	link->trb_control  = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_LINK ) | XHCI_TRB_TC );

	return( TRUE );
}

// --

// Free a transfer ring

static void XHCI_Ring_Free( struct USBBase *usbbase, struct XHCI_Ring *ring )
{
	if ( ring->trbs )
	{
		MEMORY_FREE( MEMID_HCD_4k, ring->trbs, 0 );
		ring->trbs = NULL;
	}
}

// --

// Map USB speed to XHCI speed encoding for Slot Context

static U32 XHCI_MapSpeed( U32 usb_speed )
{
	switch( usb_speed )
	{
		case USBSPEED_Full:		return( 1 );
		case USBSPEED_Low:		return( 2 );
		case USBSPEED_High:		return( 3 );
		case USBSPEED_Super:	return( 4 );
		default:				return( 1 );
	}
}

// --

// Default max packet size for EP0 based on speed

static U32 XHCI_EP0MaxPacket( U32 usb_speed )
{
	switch( usb_speed )
	{
		case USBSPEED_Low:		return( 8 );
		case USBSPEED_Full:		return( 8 );
		case USBSPEED_High:		return( 64 );
		case USBSPEED_Super:	return( 512 );
		default:				return( 8 );
	}
}

// --

// Allocate and initialize a device slot with EP0

SEC_CODE S32 XHCI_Slot_Alloc( struct USB2_HCDNode *hn, U32 slotid, U32 port, U32 usb_speed )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_InputCtrlCtx *icc;
struct XHCI_SlotCtx *sctx;
struct XHCI_EPCtx *ep0ctx;
U32 ctx_size;
U32 maxpkt;
U32 xhci_speed;
U8 *inp;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci     = & hn->hn_HCD.XHCI;
	ctx_size = xhci->ContextSize;

	// Allocate slot tracking structure
	slot = MEM_ALLOCVEC( sizeof( struct XHCI_Slot ), TRUE );

	if ( ! slot )
	{
		USBERROR( "XHCI: Slot_Alloc: out of memory for slot struct" );
		return( FALSE );
	}

	// Allocate Output Device Context (read by controller)
	// Size: (MaxEP+1) * ctx_size. We allocate a full 4K page.
	slot->output_ctx = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! slot->output_ctx )
	{
		USBERROR( "XHCI: Slot_Alloc: out of memory for output context" );
		goto fail;
	}

	slot->output_ctx_phy = ((struct Mem_FreeNode *) slot->output_ctx)->mfn_Addr;
	MEM_SET( slot->output_ctx, 0, 4096 );

	// Allocate Input Context (written by software for commands)
	// Input Context = Input Control Context + Slot Context + 31 EP Contexts
	slot->input_ctx = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! slot->input_ctx )
	{
		USBERROR( "XHCI: Slot_Alloc: out of memory for input context" );
		goto fail;
	}

	slot->input_ctx_phy = ((struct Mem_FreeNode *) slot->input_ctx)->mfn_Addr;
	MEM_SET( slot->input_ctx, 0, 4096 );

	// Allocate EP0 transfer ring
	if ( ! XHCI_Ring_Init( usbbase, & slot->transfer_ring[0] ) )
	{
		USBERROR( "XHCI: Slot_Alloc: out of memory for EP0 ring" );
		goto fail;
	}

	// -- Fill Input Context for Address Device command

	inp = (U8 *) slot->input_ctx;

	// Input Control Context (offset 0)
	icc = (struct XHCI_InputCtrlCtx *) inp;
	icc->icc_drop = 0;
	icc->icc_add  = LE_SWAP32( (1U << 0) | (1U << 1) );	// Add Slot Context (0) + EP0 (1)

	// Slot Context (offset ctx_size)
	sctx = (struct XHCI_SlotCtx *)( inp + ctx_size );

	xhci_speed = XHCI_MapSpeed( usb_speed );

	sctx->sc_info1 = LE_SWAP32(
		XHCI_SCTX_SET_SPEED( xhci_speed ) |
		XHCI_SCTX_SET_CTXENT( 1 ) );		// Context Entries = 1 (Slot + EP0)

	sctx->sc_info2 = LE_SWAP32(
		XHCI_SCTX_SET_RHPORT( port + 1 ) );	// Root Hub Port Number (1-based)

	// EP0 Context (offset ctx_size * 2, DCI=1)
	ep0ctx = (struct XHCI_EPCtx *)( inp + ctx_size * 2 );

	maxpkt = XHCI_EP0MaxPacket( usb_speed );

	ep0ctx->ep_info1 = LE_SWAP32(
		XHCI_EPCTX_SET_CERR( 3 ) );	// Error count = 3

	ep0ctx->ep_info2 = LE_SWAP32(
		XHCI_EPCTX_SET_EPTYPE( XHCI_EP_CONTROL ) |
		XHCI_EPCTX_SET_MAXPKT( maxpkt ) );

	// TR Dequeue Pointer = EP0 transfer ring physical address with DCS=1
	ep0ctx->ep_dequeue_lo = LE_SWAP32( slot->transfer_ring[0].phys | XHCI_EPCTX_DCS );
	ep0ctx->ep_dequeue_hi = 0;

	// Average TRB Length (8 bytes for control setup packets)
	ep0ctx->ep_tx_info = LE_SWAP32( XHCI_EPCTX_SET_AVGTRB( 8 ) );

	// -- Store Output Context in DCBAA

	// DCBAA entries are 64-bit LE. On 32-bit AmigaOS, high 32 bits = 0.
	// Each entry is 8 bytes, so slot N is at byte offset N*8.
	{
		U32 *dcbaa32 = (U32 *) xhci->DCBAA;
		dcbaa32[ slotid * 2 ]     = LE_SWAP32( slot->output_ctx_phy );
		dcbaa32[ slotid * 2 + 1 ] = 0;
	}

	slot->enabled = TRUE;
	xhci->Slots[ slotid ] = slot;

	usbbase->usb_IExec->DebugPrintF( "XHCI: Slot %ld allocated (port=%ld speed=%ld maxpkt=%ld)\n",
		slotid, port, usb_speed, maxpkt );

	return( TRUE );

fail:

	if ( slot->transfer_ring[0].trbs )
	{
		XHCI_Ring_Free( usbbase, & slot->transfer_ring[0] );
	}

	if ( slot->input_ctx )
	{
		MEMORY_FREE( MEMID_HCD_4k, slot->input_ctx, 0 );
	}

	if ( slot->output_ctx )
	{
		MEMORY_FREE( MEMID_HCD_4k, slot->output_ctx, 0 );
	}

	MEM_FREEVEC( slot );

	return( FALSE );
}

// --

// Free a device slot and all its resources

SEC_CODE void XHCI_Slot_Free( struct USB2_HCDNode *hn, U32 slotid )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
U32 cnt;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;
	slot = xhci->Slots[ slotid ];

	if ( ! slot )
	{
		return;
	}

	// Free all transfer rings
	for ( cnt = 0 ; cnt < 31 ; cnt++ )
	{
		if ( slot->transfer_ring[cnt].trbs )
		{
			XHCI_Ring_Free( usbbase, & slot->transfer_ring[cnt] );
		}
	}

	// Free contexts
	if ( slot->input_ctx )
	{
		MEMORY_FREE( MEMID_HCD_4k, slot->input_ctx, 0 );
	}

	if ( slot->output_ctx )
	{
		MEMORY_FREE( MEMID_HCD_4k, slot->output_ctx, 0 );
	}

	// Clear DCBAA entry
	{
		U32 *dcbaa32 = (U32 *) xhci->DCBAA;
		dcbaa32[ slotid * 2 ]     = 0;
		dcbaa32[ slotid * 2 + 1 ] = 0;
	}

	MEM_FREEVEC( slot );
	xhci->Slots[ slotid ] = NULL;
}

// --
