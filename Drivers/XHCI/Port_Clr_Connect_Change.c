
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

SEC_CODE S32 XHCI_Port_Clr_Connect_Chg( struct USB3_HCDNode *hn, U32 port )
{
struct _XHCI *xhci;
U32 val;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "XHCI : XHCI_Port_Clr_Connect_Chg" );

	xhci = & hn->hn_HCD.XHCI;

	// Read PORTSC, strip change bits and write-1-action bits, then write
	// CSC=1 to clear just the connect-status change
	val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );
	val &= ~XHCI_PS_WRITE_STRIP_MASK;
	val |= XHCI_PS_CSC;
	PCI_WRITELONG( xhci->OpBase + XHCI_PORTSC( port - 1 ), val );

	TASK_NAME_LEAVE();

	return( USB3Err_NoError );
}

// --
