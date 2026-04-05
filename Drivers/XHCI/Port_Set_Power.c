
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

SEC_CODE S32 XHCI_Port_Set_Power( struct USB2_HCDNode *hn, U32 port )
{
struct _XHCI *xhci;
U32 val;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "XHCI : XHCI_Port_Set_Power" );

	xhci = & hn->hn_HCD.XHCI;

	val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );

	// Preserve non-change bits, clear change bits, set PP
	val &= ~XHCI_PS_CHANGE_MASK;
	val |= XHCI_PS_PP;

	PCI_WRITELONG( xhci->OpBase + XHCI_PORTSC( port - 1 ), val );

	TASK_NAME_LEAVE();

	return( TRUE );
}

// --
