
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

SEC_CODE S32 XHCI_Chip_Stop( struct USB3_HCDNode *hn )
{
struct _XHCI *xhci;
U32 val;
U32 cnt;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Stop" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// Clear Run/Stop
	val = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
	val &= ~XHCI_CMD_RS;
	PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, val );

	// Wait for HCHalted
	for ( cnt = 0 ; cnt < 200 ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );

		val = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );

		if ( val & XHCI_STS_HCH )
		{
			break;
		}
	}

	// Disable interrupts
	PCI_WRITELONG( xhci->RunBase + XHCI_IMAN(0), 0 );

	usbbase->usb_IExec->DebugPrintF( "XHCI: Controller stopped\n" );

	retval = TRUE;

	TASK_NAME_LEAVE();

	return( retval );
}

// --
