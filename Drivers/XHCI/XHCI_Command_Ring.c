
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

// Enqueue a TRB onto the command ring and ring the doorbell

static S32 XHCI_Cmd_Enqueue( struct USB2_HCDNode *hn, struct XHCI_TRB *trb )
{
struct _XHCI *xhci;
struct XHCI_TRB *ring_trb;
U32 ctrl;
U32 idx;

	xhci = & hn->hn_HCD.XHCI;
	idx  = xhci->CmdRing.enqueue;

	ring_trb = & xhci->CmdRing.trbs[ idx ];

	// Write TRB fields (param and status first, control last -- cycle bit activates it)
	ring_trb->trb_param_lo = trb->trb_param_lo;
	ring_trb->trb_param_hi = trb->trb_param_hi;
	ring_trb->trb_status   = trb->trb_status;

	// Set cycle bit in control field
	ctrl = trb->trb_control;

	if ( xhci->CmdRing.cycle )
	{
		ctrl |= LE_SWAP32( XHCI_TRB_CYCLE );
	}
	else
	{
		ctrl &= ~LE_SWAP32( XHCI_TRB_CYCLE );
	}

	ring_trb->trb_control = ctrl;

	// Advance enqueue pointer
	idx++;

	// Check for Link TRB at end of ring (size-1 is the Link TRB)
	if ( idx >= ( xhci->CmdRing.size - 1 ) )
	{
		// Activate the Link TRB's cycle bit to make controller follow it
		struct XHCI_TRB *link = & xhci->CmdRing.trbs[ xhci->CmdRing.size - 1 ];

		ctrl = LE_SWAP32( link->trb_control );

		if ( xhci->CmdRing.cycle )
		{
			ctrl |= XHCI_TRB_CYCLE;
		}
		else
		{
			ctrl &= ~XHCI_TRB_CYCLE;
		}

		link->trb_control = LE_SWAP32( ctrl );

		// Toggle cycle and wrap
		xhci->CmdRing.cycle ^= 1;
		idx = 0;
	}

	xhci->CmdRing.enqueue = idx;

	// Ring the host controller command doorbell (doorbell 0, target 0)
	PCI_WRITELONG( xhci->DoorbellBase + 0, 0 );

	return( TRUE );
}

// --

// Wait for command completion event. Returns completion code.

static U32 XHCI_Cmd_Wait( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
U32 cnt;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;

	// Poll for command completion -- the HCD handler processes events
	// and sets CmdResult_Code when a command completion event arrives.
	// We poll since we might be called before the HCD task is running.

	xhci->CmdResult_Code = 0;

	for ( cnt = 0 ; cnt < 5000 ; cnt++ )
	{
		// Process any pending events inline
		XHCI_Handler_HCD( hn, 0 );

		if ( xhci->CmdResult_Code != 0 )
		{
			break;
		}

		HCD_WAIT_MS( hn, 1 );

		// Periodic heartbeat so a hung wait is visible
		if ( cnt == 100 || cnt == 500 || cnt == 1000 || cnt == 2000 )
		{
			usbbase->usb_IExec->DebugPrintF(
				"USB3: Cmd_Wait: %ld ms elapsed, no completion yet\n", cnt );
		}
	}

	if ( xhci->CmdResult_Code == 0 )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: Cmd_Wait: TIMEOUT after %ld ms - no completion event\n", cnt );
		USBERROR( "XHCI: Command timeout (no completion event after 5s)" );
		return( 0 );
	}

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Cmd_Wait: completion received after %ld ms, cc=%ld\n",
		cnt, xhci->CmdResult_Code );

	return( xhci->CmdResult_Code );
}

// --

// Enable Slot command -- returns slot ID (1-MaxSlots), or 0 on failure

SEC_CODE U32 XHCI_Cmd_EnableSlot( struct USB2_HCDNode *hn )
{
struct XHCI_TRB trb;
U32 cc;

	struct USBBase *usbbase = hn->hn_USBBase;

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_control = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_ENABLE_SLOT ) );

	XHCI_Cmd_Enqueue( hn, & trb );

	cc = XHCI_Cmd_Wait( hn );

	if ( cc != XHCI_CC_SUCCESS )
	{
		USBERROR( "XHCI: Enable Slot failed (cc=%ld)", cc );
		return( 0 );
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: Enable Slot -> slot %ld\n",
		hn->hn_HCD.XHCI.CmdResult_SlotID );

	return( hn->hn_HCD.XHCI.CmdResult_SlotID );
}

// --

// Disable Slot command

SEC_CODE S32 XHCI_Cmd_DisableSlot( struct USB2_HCDNode *hn, U32 slotid )
{
struct XHCI_TRB trb;
U32 cc;

	struct USBBase *usbbase UNUSED = hn->hn_USBBase;

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_control = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_DISABLE_SLOT ) | XHCI_TRB_SET_SLOT( slotid ) );

	XHCI_Cmd_Enqueue( hn, & trb );

	cc = XHCI_Cmd_Wait( hn );

	return( cc == XHCI_CC_SUCCESS );
}

// --

// Address Device command -- sets device address via XHCI hardware
// bsr=1: Block Set Address Request (don't assign address yet, just setup)
// bsr=0: Full address assignment

SEC_CODE S32 XHCI_Cmd_AddressDevice( struct USB2_HCDNode *hn, U32 slotid, U32 bsr )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_TRB trb;
U32 cc;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;
	slot = xhci->Slots[ slotid ];

	usbbase->usb_IExec->DebugPrintF(
		"USB3: AddressDevice ENTER slot=%ld bsr=%ld input_ctx_phy=0x%08lx\n",
		slotid, bsr, slot ? slot->input_ctx_phy : 0 );

	if ( ! slot )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: AddressDevice: slot %ld not allocated, FAIL\n", slotid );
		USBERROR( "XHCI: AddressDevice: slot %ld not allocated", slotid );
		return( FALSE );
	}

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_param_lo = LE_SWAP32( slot->input_ctx_phy );
	trb.trb_param_hi = 0;
	trb.trb_control  = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_ADDRESS_DEVICE ) |
		XHCI_TRB_SET_SLOT( slotid ) |
		( bsr ? XHCI_TRB_BSR : 0 ) );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: AddressDevice: enqueueing TRB (control=0x%08lx)\n",
		LE_SWAP32( trb.trb_control ) );

	XHCI_Cmd_Enqueue( hn, & trb );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: AddressDevice: TRB enqueued + doorbell rung, waiting for completion ...\n" );

	cc = XHCI_Cmd_Wait( hn );

	if ( cc != XHCI_CC_SUCCESS )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: AddressDevice FAILED slot=%ld bsr=%ld cc=%ld\n",
			slotid, bsr, cc );
		USBERROR( "XHCI: Address Device failed (slot=%ld bsr=%ld cc=%ld)", slotid, bsr, cc );
		return( FALSE );
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: Address Device slot %ld (bsr=%ld) -> success\n", slotid, bsr );

	return( TRUE );
}

// --

// Configure Endpoint command

SEC_CODE S32 XHCI_Cmd_ConfigureEndpoint( struct USB2_HCDNode *hn, U32 slotid )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_TRB trb;
U32 cc;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;
	slot = xhci->Slots[ slotid ];

	if ( ! slot )
	{
		USBERROR( "XHCI: ConfigureEndpoint: slot %ld not allocated", slotid );
		return( FALSE );
	}

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_param_lo = LE_SWAP32( slot->input_ctx_phy );
	trb.trb_param_hi = 0;
	trb.trb_control  = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_CONFIGURE_EP ) |
		XHCI_TRB_SET_SLOT( slotid ) );

	XHCI_Cmd_Enqueue( hn, & trb );

	cc = XHCI_Cmd_Wait( hn );

	if ( cc != XHCI_CC_SUCCESS )
	{
		USBERROR( "XHCI: Configure Endpoint failed (slot=%ld cc=%ld)", slotid, cc );
		return( FALSE );
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: Configure Endpoint slot %ld -> success\n", slotid );

	return( TRUE );
}

// --

// Evaluate Context command -- used to update max packet size after reading device descriptor

SEC_CODE S32 XHCI_Cmd_EvaluateContext( struct USB2_HCDNode *hn, U32 slotid )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_TRB trb;
U32 cc;

	struct USBBase *usbbase UNUSED = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;
	slot = xhci->Slots[ slotid ];

	if ( ! slot )
	{
		return( FALSE );
	}

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_param_lo = LE_SWAP32( slot->input_ctx_phy );
	trb.trb_param_hi = 0;
	trb.trb_control  = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_EVALUATE_CTX ) |
		XHCI_TRB_SET_SLOT( slotid ) );

	XHCI_Cmd_Enqueue( hn, & trb );

	cc = XHCI_Cmd_Wait( hn );

	return( cc == XHCI_CC_SUCCESS );
}

// --

// Reset Endpoint command -- used to clear a stalled endpoint

SEC_CODE S32 XHCI_Cmd_ResetEndpoint( struct USB2_HCDNode *hn, U32 slotid, U32 dci )
{
struct XHCI_TRB trb;
U32 cc;

	struct USBBase *usbbase UNUSED = hn->hn_USBBase;

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_control = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_RESET_EP ) |
		XHCI_TRB_SET_SLOT( slotid ) |
		XHCI_TRB_SET_EP( dci ) );

	XHCI_Cmd_Enqueue( hn, & trb );

	cc = XHCI_Cmd_Wait( hn );

	return( cc == XHCI_CC_SUCCESS );
}

// --

// Stop Endpoint command

SEC_CODE S32 XHCI_Cmd_StopEndpoint( struct USB2_HCDNode *hn, U32 slotid, U32 dci )
{
struct XHCI_TRB trb;
U32 cc;

	struct USBBase *usbbase UNUSED = hn->hn_USBBase;

	MEM_SET( & trb, 0, sizeof( trb ) );
	trb.trb_control = LE_SWAP32(
		XHCI_TRB_SET_TYPE( XHCI_TRB_STOP_EP ) |
		XHCI_TRB_SET_SLOT( slotid ) |
		XHCI_TRB_SET_EP( dci ) );

	XHCI_Cmd_Enqueue( hn, & trb );

	cc = XHCI_Cmd_Wait( hn );

	return( cc == XHCI_CC_SUCCESS || cc == XHCI_CC_STOPPED || cc == XHCI_CC_STOPPED_LENGTH );
}

// --
