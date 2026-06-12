
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

SEC_CODE S32 XHCI_Chip_Start( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
U32 val;
U32 cnt;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Start" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// -- Enable interrupter 0

	usbbase->usb_IExec->DebugPrintF( "XHCI: Start : 1 : Enable interrupts\n" );

	// Set interrupt moderation (250us = 1000 * 250ns)
	PCI_WRITELONG( xhci->RunBase + XHCI_IMOD(0), 1000 );

	// Enable interrupter
	val = PCI_READLONG( xhci->RunBase + XHCI_IMAN(0) );
	val |= XHCI_IMAN_IE;
	PCI_WRITELONG( xhci->RunBase + XHCI_IMAN(0), val );

	// -- Enable USB interrupt sources

	// Enable: Port Change, Command Complete, Transfer Complete, Host System Error
	PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD,
		XHCI_CMD_INTE | XHCI_CMD_HSEE );

	// -- Start the controller

	usbbase->usb_IExec->DebugPrintF( "XHCI: Start : 2 : Run\n" );

	val = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
	val |= XHCI_CMD_RS;
	PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, val );

	// Wait for HCHalted to clear
	for ( cnt = 0 ; cnt < 1000 ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );

		val = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );

		if ( ! ( val & XHCI_STS_HCH ) )
		{
			break;
		}
	}

	if ( val & XHCI_STS_HCH )
	{
		USBERROR( "XHCI: Start timeout -- controller did not start" );
		goto bailout;
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: Controller RUNNING\n" );

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
