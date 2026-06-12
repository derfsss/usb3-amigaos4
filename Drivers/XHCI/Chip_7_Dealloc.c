
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

SEC_CODE void XHCI_Chip_Dealloc( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;

	struct USBBase *usbbase = hn->hn_USBBase;

	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Dealloc" );

	xhci = & hn->hn_HCD.XHCI;

	// Free Slots array
	if ( xhci->Slots )
	{
		MEM_FREEVEC( xhci->Slots );
		xhci->Slots = NULL;
	}

	// Free PortResetChange
	if ( xhci->PortResetChange )
	{
		MEM_FREEVEC( xhci->PortResetChange );
		xhci->PortResetChange = NULL;
	}

	// Free ERST
	if ( xhci->ERST )
	{
		MEMORY_FREE( MEMID_HCD_4k, xhci->ERST, 0 );
		xhci->ERST = NULL;
	}

	// Free Event Ring
	if ( xhci->EvtRing.trbs )
	{
		MEMORY_FREE( MEMID_HCD_4k, xhci->EvtRing.trbs, 0 );
		xhci->EvtRing.trbs = NULL;
	}

	// Free Command Ring
	if ( xhci->CmdRing.trbs )
	{
		MEMORY_FREE( MEMID_HCD_4k, xhci->CmdRing.trbs, 0 );
		xhci->CmdRing.trbs = NULL;
	}

	// Free DCBAA
	if ( xhci->DCBAA )
	{
		MEMORY_FREE( MEMID_HCD_4k, xhci->DCBAA, 0 );
		xhci->DCBAA = NULL;
	}

	// Free signals
	TASK_FREESIGNAL( & xhci->Signal_Command );
	TASK_FREESIGNAL( & xhci->Signal_Event );

	TASK_NAME_LEAVE();
}

// --
