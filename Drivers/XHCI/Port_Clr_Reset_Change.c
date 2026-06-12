
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

SEC_CODE S32 XHCI_Port_Clr_Reset_Chg( struct USB3_HCDNode *hn, U32 port )
{
struct _XHCI *xhci;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "XHCI : XHCI_Port_Clr_Reset_Chg" );

	xhci = & hn->hn_HCD.XHCI;

	// Clear software reset change flag
	xhci->PortResetChange[ port ] = 0;

	TASK_NAME_LEAVE();

	return( USB3Err_NoError );
}

// --
