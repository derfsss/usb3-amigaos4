
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

// XHCI handles SET_ADDRESS internally via the Address Device command.
// When the stack sends a SET_ADDRESS control transfer, we intercept it
// here and perform Enable Slot + Slot_Alloc + Address Device instead.

static S32 XHCI_Handle_SetAddress( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct _XHCI *xhci;
struct RealFunctionNode *fn;
U32 slotid;
U32 port;
U32 speed;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;
	fn   = ioreq->req_Function;

	port  = fn->fkt_PortNr;
	speed = fn->fkt_Speed;

	usbbase->usb_IExec->DebugPrintF( "XHCI: SET_ADDRESS intercepted (port=%ld speed=%ld)\n", port, speed );

	// Enable a device slot
	slotid = XHCI_Cmd_EnableSlot( hn );

	if ( ! slotid )
	{
		USBERROR( "XHCI: SET_ADDRESS: Enable Slot failed" );
		ioreq->req_Public.io_Error = USB2Err_Host_HostError;
		return( TRUE );	// handled (with error)
	}

	// Allocate slot resources (contexts, EP0 ring)
	if ( ! XHCI_Slot_Alloc( hn, slotid, port, speed ) )
	{
		USBERROR( "XHCI: SET_ADDRESS: Slot Alloc failed" );
		XHCI_Cmd_DisableSlot( hn, slotid );
		ioreq->req_Public.io_Error = USB2Err_Stack_NoMemory;
		return( TRUE );
	}

	// Issue Address Device command (bsr=0 for full address assignment)
	if ( ! XHCI_Cmd_AddressDevice( hn, slotid, 0 ) )
	{
		USBERROR( "XHCI: SET_ADDRESS: Address Device failed" );
		XHCI_Slot_Free( hn, slotid );
		XHCI_Cmd_DisableSlot( hn, slotid );
		ioreq->req_Public.io_Error = USB2Err_Host_HostError;
		return( TRUE );
	}

	// Success — the XHCI controller has assigned an address.
	// The stack expects this to work like a normal SET_ADDRESS success.
	ioreq->req_Public.io_Error = 0;
	ioreq->req_Public.io_Actual = 0;

	usbbase->usb_IExec->DebugPrintF( "XHCI: SET_ADDRESS complete (slot=%ld)\n", slotid );

	return( FALSE );	// not handled by normal HCD path — we did it inline
}

// --

SEC_CODE S32 XHCI_Control_Build( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB2_SetupData *setup;
S32 handled;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "XHCI : XHCI_Control_Build" );

	handled = TRUE;

	setup = ioreq->req_Public.io_SetupData;

	if ( ! setup )
	{
		USBERROR( "XHCI: Control_Build: no setup data" );
		ioreq->req_Public.io_Error = USB2Err_Stack_InvalidStructure;
		goto bailout;
	}

	// Check if this is a SET_ADDRESS request — XHCI handles this internally
	if (( setup->RequestType == ( REQTYPE_Write | REQTYPE_Standard | REQTYPE_Device ) )
	&&	( setup->RequestCode == REQCODE_Set_Address ))
	{
		handled = XHCI_Handle_SetAddress( hn, ioreq );
		goto bailout;
	}

	// TODO Phase 3c: Build Setup/Data/Status TRBs for other control transfers
	// For now, return error for unsupported transfers
	USBERROR( "XHCI: Control_Build: not yet implemented (req=0x%02lx)", (U32) setup->RequestCode );
	ioreq->req_Public.io_Error = USB2Err_Host_UnknownError;

bailout:

	TASK_NAME_LEAVE();

	return( handled );
}

// --
