
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

SEC_CODE S32 XHCI_Chip_Alloc( struct USB3_HCDNode *hn )
{
struct _XHCI *xhci;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Alloc" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// -- Allocate signals

	usbbase->usb_IExec->DebugPrintF( "XHCI: Alloc : 1 : Signals\n" );

	if ( ! TASK_ALLOCSIGNAL( & xhci->Signal_Event ))
	{
		USBERROR( "XHCI: Error allocating Event signal" );
		goto bailout;
	}

	if ( ! TASK_ALLOCSIGNAL( & xhci->Signal_Command ))
	{
		USBERROR( "XHCI: Error allocating Command signal" );
		goto bailout;
	}

	// -- Allocate DCBAA (Device Context Base Address Array)
	// MaxSlots+1 entries of 8 bytes each (slot 0 = scratchpad)
	// Use 4K-aligned DMA buffer

	usbbase->usb_IExec->DebugPrintF( "XHCI: Alloc : 2 : DCBAA\n" );

	xhci->DCBAA = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! xhci->DCBAA )
	{
		USBERROR( "XHCI: Error allocating DCBAA" );
		goto bailout;
	}

	xhci->DCBAA_Phyaddr = ((struct Mem_FreeNode *) xhci->DCBAA)->mfn_Addr;

	// -- Allocate Command Ring (256 TRBs)

	usbbase->usb_IExec->DebugPrintF( "XHCI: Alloc : 3 : Command Ring\n" );

	xhci->CmdRing.trbs = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! xhci->CmdRing.trbs )
	{
		USBERROR( "XHCI: Error allocating Command Ring" );
		goto bailout;
	}

	xhci->CmdRing.phys = ((struct Mem_FreeNode *) xhci->CmdRing.trbs)->mfn_Addr;
	xhci->CmdRing.size = 4096 / XHCI_TRB_SIZE;	// 256 TRBs
	xhci->CmdRing.enqueue = 0;
	xhci->CmdRing.cycle = 1;

	// -- Allocate Event Ring (256 TRBs)

	usbbase->usb_IExec->DebugPrintF( "XHCI: Alloc : 4 : Event Ring\n" );

	xhci->EvtRing.trbs = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! xhci->EvtRing.trbs )
	{
		USBERROR( "XHCI: Error allocating Event Ring" );
		goto bailout;
	}

	xhci->EvtRing.phys = ((struct Mem_FreeNode *) xhci->EvtRing.trbs)->mfn_Addr;
	xhci->EvtRing.size = 4096 / XHCI_TRB_SIZE;
	xhci->EvtRing.dequeue = 0;
	xhci->EvtRing.cycle = 1;

	// -- Allocate Event Ring Segment Table (1 entry)

	usbbase->usb_IExec->DebugPrintF( "XHCI: Alloc : 5 : ERST\n" );

	xhci->ERST = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

	if ( ! xhci->ERST )
	{
		USBERROR( "XHCI: Error allocating ERST" );
		goto bailout;
	}

	xhci->ERST_Phyaddr = ((struct Mem_FreeNode *) xhci->ERST)->mfn_Addr;

	// -- Allocate Port Reset Change tracking

	usbbase->usb_IExec->DebugPrintF( "XHCI: Alloc : 6 : Port tracking\n" );

	xhci->PortResetChange = MEM_ALLOCVEC( xhci->MaxPorts + 1, TRUE );

	if ( ! xhci->PortResetChange )
	{
		USBERROR( "XHCI: Error allocating PortResetChange" );
		goto bailout;
	}

	// -- Allocate Slots array

	xhci->Slots = MEM_ALLOCVEC( sizeof( struct XHCI_Slot * ) * ( xhci->MaxSlots + 1 ), TRUE );

	if ( ! xhci->Slots )
	{
		USBERROR( "XHCI: Error allocating Slots" );
		goto bailout;
	}

	// -- Set HCD signal mask

	hn->HCD_Mask = xhci->Signal_Event.sig_Signal_Mask | xhci->Signal_Command.sig_Signal_Mask;

	usbbase->usb_IExec->DebugPrintF( "XHCI: Alloc : Done\n" );

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
