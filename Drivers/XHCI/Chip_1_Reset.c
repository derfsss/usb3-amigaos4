
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

SEC_CODE S32 XHCI_Chip_Reset( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
U32 val;
U32 cnt;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Reset" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// -- Halt the controller if running

	val = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );

	if ( val & XHCI_CMD_RS )
	{
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

		if ( ! ( val & XHCI_STS_HCH ) )
		{
			USBERROR( "XHCI: Halt timeout" );
			goto bailout;
		}
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: Controller halted\n" );

	// -- Reset the controller

	val = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
	val |= XHCI_CMD_HCRST;
	PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, val );

	// Wait for HCRST to clear
	for ( cnt = 0 ; cnt < 1000 ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );

		val = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );

		if ( ! ( val & XHCI_CMD_HCRST ) )
		{
			break;
		}
	}

	if ( val & XHCI_CMD_HCRST )
	{
		USBERROR( "XHCI: Reset timeout" );
		goto bailout;
	}

	// Wait for Controller Not Ready to clear
	for ( cnt = 0 ; cnt < 1000 ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );

		val = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );

		if ( ! ( val & XHCI_STS_CNR ) )
		{
			break;
		}
	}

	if ( val & XHCI_STS_CNR )
	{
		USBERROR( "XHCI: CNR timeout" );
		goto bailout;
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: Controller reset complete\n" );

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
