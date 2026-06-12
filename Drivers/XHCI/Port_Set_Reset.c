
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

static const char *xhci_speed_name( U32 speed )
{
	switch( speed )
	{
		case 0:                 return( "Undefined" );
		case XHCI_PS_SPEED_FS:  return( "Full-Speed (12 Mbps)" );
		case XHCI_PS_SPEED_LS:  return( "Low-Speed (1.5 Mbps)" );
		case XHCI_PS_SPEED_HS:  return( "High-Speed (480 Mbps)" );
		case XHCI_PS_SPEED_SS:  return( "SuperSpeed (5 Gbps)" );
		default:                return( "Reserved/PSI-defined" );
	}
}

static const char *xhci_pls_name( U32 pls )
{
	switch( pls )
	{
		case 0:  return( "U0 (Active)" );
		case 1:  return( "U1" );
		case 2:  return( "U2" );
		case 3:  return( "U3 (Suspend)" );
		case 4:  return( "Disabled" );
		case 5:  return( "RxDetect" );
		case 6:  return( "Inactive" );
		case 7:  return( "Polling" );
		case 8:  return( "Recovery" );
		case 9:  return( "Hot Reset" );
		case 10: return( "Compliance" );
		case 11: return( "Test/Loopback" );
		case 15: return( "Resume" );
		default: return( "Reserved" );
	}
}

// --
// CURRENTLY UNUSED -- kept for reference from the rolled-back BSR=1
// double-step experiment (AddressDevice(BSR=1) -> GET_DESCRIPTOR(8) ->
// AddressDevice(BSR=0), the pattern Windows uses). On the uPD720202,
// BSR=1 leaves EP0 in a non-Running state and EP0 doorbell rings are
// silently ignored, so the injected transfer always timed out. We now
// use the single-step AddressDevice(BSR=0) flow (Linux's default) and
// no injection is needed.
//
// Injects a GET_DESCRIPTOR(Device, 8 bytes) over the slot's EP0 ring,
// polling XHCI_Handler_HCD for the Transfer Event (no IORequest needed,
// uses the slot's LastTransfer_Done/CC fields).

static UNUSED void XHCI_DefaultGetDescriptor8( struct USB2_HCDNode *hn, U32 slotid )
{
struct _XHCI *xhci = & hn->hn_HCD.XHCI;
struct USBBase *usbbase = hn->hn_USBBase;
struct XHCI_Slot *slot;
struct XHCI_Ring *ring;
struct XHCI_TRB trb;
PTR  buf;
U32  buf_phy;
U32  cnt;

	slot = xhci->Slots[ slotid ];
	if ( ! slot )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: DefaultGetDesc: slot %ld missing, skipping\n", slotid );
		return;
	}

	ring = & slot->transfer_ring[0];	// EP0 = DCI 1, transfer_ring[0]

	// Allocate a small DMA buffer (we use the smallest HCD pool, 4 KB).
	// xHCI doesn't care about the size; we only need 8 bytes.
	buf = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );
	if ( ! buf )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: DefaultGetDesc: out of memory for buffer, skipping\n" );
		return;
	}
	buf_phy = ((struct Mem_FreeNode *) buf)->mfn_Addr;
	MEM_SET( buf, 0, 8 );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: DefaultGetDesc: slot=%ld buf=%p buf_phy=0x%08lx -- enqueueing setup/data/status TRBs on EP0\n",
		slotid, buf, buf_phy );

	// -- Setup Stage TRB (immediate data, transfer type = IN data follows)
	// Setup packet bytes (USB wire LE order):
	//   00: bmRequestType = 0x80 (D2H | Standard | Device)
	//   01: bRequest      = 0x06 (GET_DESCRIPTOR)
	//   02: wValue lo     = 0x00 (descriptor index 0)
	//   03: wValue hi     = 0x01 (descriptor type 1 = DEVICE)
	//   04: wIndex lo     = 0x00
	//   05: wIndex hi     = 0x00
	//   06: wLength lo    = 0x08
	//   07: wLength hi    = 0x00
	{
	U8 setup[8] = { 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x08, 0x00 };
	U32 *sd32 = (U32 *) setup;

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_param_lo = sd32[0];
	trb.trb_param_hi = sd32[1];
	trb.trb_status   = LE_SWAP32( XHCI_TRB_SET_XFERLEN( 8 ) | XHCI_TRB_SET_INTR( 0 ) );
	trb.trb_control  = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_SETUP ) |
		XHCI_TRB_IDT |
		XHCI_TRB_TRT_IN );

	XHCI_Ring_Enqueue_TRB( hn, ring, & trb );
	}

	// -- Data Stage TRB (IN, 8 bytes into our buffer)
	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_param_lo = LE_SWAP32( buf_phy );
	trb.trb_param_hi = 0;
	trb.trb_status   = LE_SWAP32(
		XHCI_TRB_SET_XFERLEN( 8 ) |
		XHCI_TRB_SET_TDSIZE( 0 ) |
		XHCI_TRB_SET_INTR( 0 ) );
	trb.trb_control  = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_DATA ) |
		XHCI_TRB_DIR_IN );
	XHCI_Ring_Enqueue_TRB( hn, ring, & trb );

	// -- Status Stage TRB (OUT, IOC=1 so we get a Transfer Event)
	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_status  = 0;
	trb.trb_control = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_STATUS ) |
		XHCI_TRB_IOC );
	XHCI_Ring_Enqueue_TRB( hn, ring, & trb );

	// -- Clear pending flag, ring the EP0 doorbell (target DCI 1)
	slot->LastTransfer_Done = 0;
	slot->LastTransfer_CC   = 0;

	usbbase->usb_IExec->DebugPrintF(
		"USB3: DefaultGetDesc: ringing doorbell slot=%ld DCI=1 (EP0)\n", slotid );
	PCI_WRITELONG( xhci->DoorbellBase + 4 * slotid, 1 );

	// -- Poll for transfer completion (up to 500 ms)
	for ( cnt = 0 ; cnt < 500 ; cnt++ )
	{
		XHCI_Handler_HCD( hn, 0 );
		if ( slot->LastTransfer_Done ) break;
		HCD_WAIT_MS( hn, 1 );
	}

	if ( ! slot->LastTransfer_Done )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: DefaultGetDesc: TIMEOUT after 500 ms - no transfer event for slot %ld\n",
			slotid );
	}
	else
	{
		U8 *b = (U8 *) buf;
		usbbase->usb_IExec->DebugPrintF(
			"USB3: DefaultGetDesc: completed in %ld ms cc=%ld, first 8 bytes = "
			"%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx\n",
			cnt, (U32) slot->LastTransfer_CC,
			(U32) b[0], (U32) b[1], (U32) b[2], (U32) b[3],
			(U32) b[4], (U32) b[5], (U32) b[6], (U32) b[7] );
	}

	MEMORY_FREE( MEMID_HCD_4k, buf, 0 );
}

// --

SEC_CODE S32 XHCI_Port_Set_Reset( struct USB2_HCDNode *hn, U32 port )
{
struct _XHCI *xhci;
U32 val;
U32 cnt;
U32 speed;
U32 pls;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Port_Set_Reset" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );

	speed = ( val & XHCI_PS_SPEED_MASK ) >> XHCI_PS_SPEED_SHIFT;
	pls   = ( val & XHCI_PS_PLS_MASK ) >> XHCI_PS_PLS_SHIFT;

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Port %ld pre-reset:  PORTSC=0x%08lx PLS=%ld(%s) speed=%ld(%s) CCS=%ld PED=%ld\n",
		port, val, pls, xhci_pls_name( pls ),
		speed, xhci_speed_name( speed ),
		( val & XHCI_PS_CCS ) ? 1 : 0, ( val & XHCI_PS_PED ) ? 1 : 0 );

	// Set Port Reset bit (strip change bits and write-1-action bits first)
	val &= ~XHCI_PS_WRITE_STRIP_MASK;
	val |= XHCI_PS_PR;
	PCI_WRITELONG( xhci->OpBase + XHCI_PORTSC( port - 1 ), val );

	// Wait for Port Reset Change
	for ( cnt = 0 ; cnt < 500 ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );

		val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );

		if ( val & XHCI_PS_PRC )
		{
			break;
		}
	}

	speed = ( val & XHCI_PS_SPEED_MASK ) >> XHCI_PS_SPEED_SHIFT;
	pls   = ( val & XHCI_PS_PLS_MASK ) >> XHCI_PS_PLS_SHIFT;

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Port %ld post-reset: PORTSC=0x%08lx PLS=%ld(%s) speed=%ld(%s) PRC-wait=%ld ms PED=%ld\n",
		port, val, pls, xhci_pls_name( pls ),
		speed, xhci_speed_name( speed ), cnt,
		( val & XHCI_PS_PED ) ? 1 : 0 );

	if ( ! ( val & XHCI_PS_PRC ) )
	{
		USBERROR( "XHCI: Port %ld reset timeout", port );
		goto bailout;
	}

	// Snapshot PED + speed from this post-reset frame BEFORE we touch PRC.
	//
	// History: the "Renesas drops PED+speed after the PRC clear" behaviour
	// seen on the uPD720202 was (at least partly) self-inflicted -- the PRC
	// clear write used to preserve PED=1, and PED is RW1C, so the write
	// itself disabled the port (and a disabled port reads speed=0). That
	// is fixed now via XHCI_PS_WRITE_STRIP_MASK, but the snapshot is kept:
	// it is free, and it makes the enable/speed decision immune to any
	// remaining post-reset PORTSC weirdness on this silicon.
	{
	U32 ped_at_postreset = ( val & XHCI_PS_PED ) ? 1 : 0;
	U32 speed_at_postreset = speed;

	// Clear PRC (write 1 to clear; strip PED/PR/LWS so the write is neutral)
	val &= ~XHCI_PS_WRITE_STRIP_MASK;
	val |= XHCI_PS_PRC;
	PCI_WRITELONG( xhci->OpBase + XHCI_PORTSC( port - 1 ), val );

	// Mark software reset change
	xhci->PortResetChange[ port ] = 1;

	// Re-read for diagnostic only -- actual decision uses the post-reset snapshot
	val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Port %ld after-PRC-clear: PORTSC=0x%08lx PED=%ld (post-reset PED was %ld)\n",
		port, val,
		( val & XHCI_PS_PED ) ? 1 : 0, ped_at_postreset );

	if ( ped_at_postreset )
	{
		U32 usb_speed;

		speed = speed_at_postreset;

		// Map XHCI speed to USB speed enum
		switch( speed )
		{
			case XHCI_PS_SPEED_FS: usb_speed = USBSPEED_Full; break;
			case XHCI_PS_SPEED_LS: usb_speed = USBSPEED_Low; break;
			case XHCI_PS_SPEED_HS: usb_speed = USBSPEED_High; break;
			case XHCI_PS_SPEED_SS: usb_speed = USBSPEED_Super; break;
			default:               usb_speed = USBSPEED_Full; break;
		}

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Port %ld negotiated speed=%ld(%s) -> usb_speed=%ld\n",
			port, speed, xhci_speed_name( speed ), usb_speed );

		// CACHE the negotiated speed. The Renesas chip drops PORTSC.speed
		// back to 0 after we clear PRC, so by the time Control_Build
		// intercepts SET_ADDRESS the speed bits are gone. We must capture
		// it here while it's still valid.
		if ( port < ( sizeof( xhci->PortSpeed ) / sizeof( xhci->PortSpeed[0] ) ) )
		{
			xhci->PortSpeed[ port ] = (U8) usb_speed;
		}

		// Slot creation deferred to SET_ADDRESS interception. Matches
		// the standard Linux single-step AddressDevice(BSR=0) flow.
		usbbase->usb_IExec->DebugPrintF(
			"USB3: Port %ld reset OK - cached usb_speed=%ld, slot creation deferred to SET_ADDRESS\n",
			port, usb_speed );

		// Mark address-0 mapping as "no slot yet" so Control_Build takes
		// the fallback EnableSlot+Slot_Alloc path.
		xhci->SlotID_ByAddress[0] = 0;

		retval = USB2Err_NoError;
	}
	else
	{
		USBERROR( "XHCI: Port %ld not enabled after reset (PORTSC=0x%08lx)", port, val );
	}
	}	// end of post-reset-snapshot scope

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
