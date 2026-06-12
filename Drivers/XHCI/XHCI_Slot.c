
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

	// Place Link TRB at last position. CHAIN is set so a multi-TRB TD
	// (chained bulk) may legally span the ring wrap; it is harmless for
	// single-TRB TDs since the link TRB then sits between TDs.
	link = & ring->trbs[ ring->size - 1 ];
	link->trb_param_lo = LE_SWAP32( ring->phys );
	link->trb_param_hi = 0;
	link->trb_status   = 0;
	link->trb_control  = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_LINK ) | XHCI_TRB_TC | XHCI_TRB_CHAIN );

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

// Enqueue a TRB onto a transfer ring (does NOT ring doorbell)
// Returns physical address of the enqueued TRB, or 0 on failure

SEC_CODE U32 XHCI_Ring_Enqueue_TRB( struct USB2_HCDNode *hn UNUSED, struct XHCI_Ring *ring, struct XHCI_TRB *trb )
{
struct XHCI_TRB *ring_trb;
U32 ctrl;
U32 idx;
U32 phy;

	idx = ring->enqueue;
	phy = ring->phys + ( idx * XHCI_TRB_SIZE );

	ring_trb = & ring->trbs[ idx ];

	// Write param and status first, control last (cycle bit activates)
	ring_trb->trb_param_lo = trb->trb_param_lo;
	ring_trb->trb_param_hi = trb->trb_param_hi;
	ring_trb->trb_status   = trb->trb_status;

	ctrl = trb->trb_control;

	if ( ring->cycle )
	{
		ctrl |= LE_SWAP32( XHCI_TRB_CYCLE );
	}
	else
	{
		ctrl &= ~LE_SWAP32( XHCI_TRB_CYCLE );
	}

	ring_trb->trb_control = ctrl;

	// Advance enqueue
	idx++;

	// Check for Link TRB at end of ring
	if ( idx >= ( ring->size - 1 ) )
	{
		struct XHCI_TRB *link = & ring->trbs[ ring->size - 1 ];

		ctrl = LE_SWAP32( link->trb_control );

		if ( ring->cycle )
		{
			ctrl |= XHCI_TRB_CYCLE;
		}
		else
		{
			ctrl &= ~XHCI_TRB_CYCLE;
		}

		link->trb_control = LE_SWAP32( ctrl );

		ring->cycle ^= 1;
		idx = 0;
	}

	ring->enqueue = idx;

	return( phy );
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

// Compute XHCI Device Context Index from endpoint number + direction

SEC_CODE U32 XHCI_Endpoint_DCI( U32 ep_number, U32 ep_direction )
{
	if ( ep_number == 0 )
	{
		return( 1 );	// EP0 is always DCI 1 (bidirectional control)
	}

	// DCI = ep_number * 2 + direction (0=OUT, 1=IN)
	return( ep_number * 2 + ( ( ep_direction & EPDIR_In ) ? 1 : 0 ) );
}

// --

// Set up endpoint contexts for all non-EP0 endpoints in a configuration.
// Called when SET_CONFIGURATION is intercepted.
// Builds the Input Context with endpoint contexts and issues Configure Endpoint.

SEC_CODE S32 XHCI_Slot_ConfigureEndpoints( struct USB2_HCDNode *hn, U32 slotid, struct RealFunctionNode *fn )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_InputCtrlCtx *icc;
struct XHCI_SlotCtx *sctx;
struct XHCI_EPCtx *epctx;
struct USB2_ConfigNode *cfg;
struct USB2_InterfaceHeader *ifh;
struct USB2_InterfaceNode *ifn;
struct USB2_EndPointNode *ep;
// Note: USB2_InterfaceGroup declared in scope below
U32 ctx_size;
U32 dci;
U32 max_dci;
U32 add_flags;
U32 xhci_ep_type;
U8 *inp;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci     = & hn->hn_HCD.XHCI;
	slot     = xhci->Slots[ slotid ];
	ctx_size = xhci->ContextSize;

	if ( ! slot )
	{
		USBERROR( "XHCI: ConfigureEndpoints: slot %ld not found", slotid );
		return( FALSE );
	}

	// Clear Input Context
	inp = (U8 *) slot->input_ctx;
	MEM_SET( inp, 0, 4096 );

	add_flags = (1U << 0);	// Always add Slot Context (bit 0)
	max_dci   = 1;			// At least EP0

	// Walk all endpoints in the active configuration
	cfg = fn->fkt_Config_Active;

	if ( ! cfg )
	{
		USBERROR( "XHCI: ConfigureEndpoints: no active config" );
		return( FALSE );
	}

	// Walk interface groups -> interface headers -> active interface node -> endpoints
	{
	struct USB2_InterfaceGroup *ig;

	ig = cfg->cfg_InterfaceGroups.uh_Head;

	while( ig )
	{
		ifh = ig->ig_Group.uh_Head;

		while( ifh )
		{
			ifn = ifh->ih_Active;

			if ( ifn )
			{
			ep = ifn->in_EndPoints.uh_Head;

			while( ep )
			{
				if ( ep->ep_Number == 0 )
				{
					ep = Node_Next( ep );
					continue;	// Skip EP0
				}

				dci = XHCI_Endpoint_DCI( ep->ep_Number, ep->ep_Direction );

				if ( dci > 31 || dci < 2 )
				{
					ep = Node_Next( ep );
					continue;
				}

				// Allocate transfer ring for this endpoint if needed
				if ( ! slot->transfer_ring[ dci - 1 ].trbs )
				{
					if ( ! XHCI_Ring_Init( usbbase, & slot->transfer_ring[ dci - 1 ] ) )
					{
						USBERROR( "XHCI: ConfigureEndpoints: ring alloc failed for DCI %ld", dci );
						return( FALSE );
					}
				}

				// Determine XHCI EP type
				if ( ep->ep_Type == EPATT_Type_Bulk )
				{
					xhci_ep_type = ( ep->ep_Direction & EPDIR_In ) ? XHCI_EP_BULK_IN : XHCI_EP_BULK_OUT;
				}
				else if ( ep->ep_Type == EPATT_Type_Interrupt )
				{
					xhci_ep_type = ( ep->ep_Direction & EPDIR_In ) ? XHCI_EP_INTR_IN : XHCI_EP_INTR_OUT;
				}
				else if ( ep->ep_Type == EPATT_Type_Isochronous )
				{
					xhci_ep_type = ( ep->ep_Direction & EPDIR_In ) ? XHCI_EP_ISOCH_IN : XHCI_EP_ISOCH_OUT;
				}
				else
				{
					xhci_ep_type = XHCI_EP_CONTROL;
				}

				// Fill Endpoint Context in Input Context
				epctx = (struct XHCI_EPCtx *)( inp + ctx_size * ( dci + 1 ) );

				epctx->ep_info1 = LE_SWAP32(
					XHCI_EPCTX_SET_CERR( 3 ) |
					XHCI_EPCTX_SET_INTERVAL( ep->ep_Interval > 0 ? ep->ep_Interval : 0 ) );

				epctx->ep_info2 = LE_SWAP32(
					XHCI_EPCTX_SET_EPTYPE( xhci_ep_type ) |
					XHCI_EPCTX_SET_MAXPKT( ep->ep_MaxPacketSize ) );

				epctx->ep_dequeue_lo = LE_SWAP32( slot->transfer_ring[ dci - 1 ].phys | XHCI_EPCTX_DCS );
				epctx->ep_dequeue_hi = 0;

				epctx->ep_tx_info = LE_SWAP32(
					XHCI_EPCTX_SET_AVGTRB( ep->ep_MaxPacketSize ) |
					XHCI_EPCTX_SET_MAXESIT( ep->ep_MaxPacketSize ) );

				add_flags |= (1U << dci);

				if ( dci > max_dci )
				{
					max_dci = dci;
				}

				usbbase->usb_IExec->DebugPrintF( "XHCI: ConfigEP: DCI=%ld type=%ld maxpkt=%ld interval=%ld\n",
					dci, xhci_ep_type, ep->ep_MaxPacketSize, ep->ep_Interval );

				ep = Node_Next( ep );
			}
			}	// if ifn

			ifh = Node_Next( ifh );
		}

		ig = Node_Next( ig );
	}
	}	// interface group scope

	// Fill Input Control Context
	icc = (struct XHCI_InputCtrlCtx *) inp;
	icc->icc_drop = 0;
	icc->icc_add  = LE_SWAP32( add_flags );

	// Update Slot Context with max DCI
	sctx = (struct XHCI_SlotCtx *)( inp + ctx_size );
	sctx->sc_info1 = LE_SWAP32( XHCI_SCTX_SET_CTXENT( max_dci ) );

	usbbase->usb_IExec->DebugPrintF( "XHCI: ConfigureEndpoints: slot=%ld max_dci=%ld add=0x%08lx\n",
		slotid, max_dci, add_flags );

	// Issue Configure Endpoint command
	return( XHCI_Cmd_ConfigureEndpoint( hn, slotid ) );
}

// --
