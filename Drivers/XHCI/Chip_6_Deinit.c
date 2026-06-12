
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

SEC_CODE void XHCI_Chip_Deinit( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
U32 cnt;

	struct USBBase *usbbase = hn->hn_USBBase;

	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Deinit" );

	xhci = & hn->hn_HCD.XHCI;

	// Free scratchpad pages
	if ( xhci->ScratchpadPages )
	{
		for ( cnt = 0 ; cnt < xhci->ScratchpadCount2 ; cnt++ )
		{
			if ( xhci->ScratchpadPages[cnt] )
			{
				MEMORY_FREE( MEMID_HCD_4k, xhci->ScratchpadPages[cnt], 0 );
				xhci->ScratchpadPages[cnt] = NULL;
			}
		}

		MEM_FREEVEC( xhci->ScratchpadPages );
		xhci->ScratchpadPages = NULL;
	}

	// Free scratchpad array
	if ( xhci->ScratchpadArray )
	{
		MEMORY_FREE( MEMID_HCD_4k, xhci->ScratchpadArray, 0 );
		xhci->ScratchpadArray = NULL;
	}

	TASK_NAME_LEAVE();
}

// --
