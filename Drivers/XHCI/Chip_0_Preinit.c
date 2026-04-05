
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

SEC_CODE S32 XHCI_Chip_Preinit( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
U32 hcs1;
U32 hcs2;
U32 hcc1;
U32 val;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Preinit" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// Read Capability Register Length
	val = PCI_READBYTE( XHCI_CAPLENGTH );
	xhci->CapLength = val;

	usbbase->usb_IExec->DebugPrintF( "XHCI: CapLength = %ld\n", val );

	// Compute register base offsets
	xhci->OpBase       = xhci->CapLength;
	xhci->DoorbellBase = PCI_READLONG( XHCI_DBOFF ) & ~0x3U;
	xhci->RunBase      = PCI_READLONG( XHCI_RTSOFF ) & ~0x1FU;

	usbbase->usb_IExec->DebugPrintF( "XHCI: OpBase=0x%lx RunBase=0x%lx DoorbellBase=0x%lx\n",
		xhci->OpBase, xhci->RunBase, xhci->DoorbellBase );

	// Read Structural Parameters 1
	hcs1 = PCI_READLONG( XHCI_HCSPARAMS1 );

	xhci->MaxSlots = XHCI_HCS1_MAXSLOTS( hcs1 );
	xhci->MaxIntrs = XHCI_HCS1_MAXINTRS( hcs1 );
	xhci->MaxPorts = XHCI_HCS1_MAXPORTS( hcs1 );

	usbbase->usb_IExec->DebugPrintF( "XHCI: MaxSlots=%ld MaxIntrs=%ld MaxPorts=%ld\n",
		xhci->MaxSlots, xhci->MaxIntrs, xhci->MaxPorts );

	if ( xhci->MaxPorts == 0 )
	{
		USBERROR( "XHCI: MaxPorts is 0 — aborting" );
		goto bailout;
	}

	// Read Structural Parameters 2 — scratchpad count
	hcs2 = PCI_READLONG( XHCI_HCSPARAMS2 );
	xhci->ScratchpadCount = XHCI_HCS2_MAXSCRATCH( hcs2 );

	usbbase->usb_IExec->DebugPrintF( "XHCI: ScratchpadCount=%ld\n", xhci->ScratchpadCount );

	// Read Capability Parameters 1
	hcc1 = PCI_READLONG( XHCI_HCCPARAMS1 );

	// Context size: 32 bytes (CSZ=0) or 64 bytes (CSZ=1)
	xhci->ContextSize = ( hcc1 & XHCI_HCC1_CSZ ) ? 64 : 32;

	usbbase->usb_IExec->DebugPrintF( "XHCI: ContextSize=%ld AC64=%ld\n",
		xhci->ContextSize, ( hcc1 & XHCI_HCC1_AC64 ) ? 1 : 0 );

	// Read page size from operational registers
	val = PCI_READLONG( xhci->OpBase + XHCI_PAGESIZE );
	xhci->PageSize = ( val & 0xFFFF ) << 12;	// Bit N set means 2^(N+12) bytes

	if ( xhci->PageSize == 0 )
	{
		xhci->PageSize = 4096;
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: PageSize=%ld\n", xhci->PageSize );

	// Set root hub port count for the stack
	hn->hn_HUB_NumPorts = xhci->MaxPorts;

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
