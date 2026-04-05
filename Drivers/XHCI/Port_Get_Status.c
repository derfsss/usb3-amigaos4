
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

SEC_CODE U32 XHCI_Port_Get_Status( struct USB2_HCDNode *hn, U32 port )
{
struct _XHCI *xhci;
U32 status;
U32 retval;
U32 speed;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Port_Get_Status" );

	xhci = & hn->hn_HCD.XHCI;
	retval = 0;

	// Port numbers from HUB driver are 1-based, XHCI PORTSC is 0-based
	status = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );

	usbbase->usb_IExec->DebugPrintF( "XHCI: Port_Get_Status(%ld) PORTSC=0x%08lx\n", port, status );

	// Connection
	if ( status & XHCI_PS_CCS )
	{
		retval |= HUBF_Status_Connection;
	}

	// Enabled
	if ( status & XHCI_PS_PED )
	{
		retval |= HUBF_Status_Enable;
	}

	// Over-current
	if ( status & XHCI_PS_OCA )
	{
		retval |= HUBF_Status_Over_Current;
	}

	// Reset
	if ( status & XHCI_PS_PR )
	{
		retval |= HUBF_Status_Reset;
	}

	// Power
	if ( status & XHCI_PS_PP )
	{
		retval |= HUBF_Status_Power;
	}

	// Speed
	speed = ( status & XHCI_PS_SPEED_MASK ) >> XHCI_PS_SPEED_SHIFT;

	if ( speed == XHCI_PS_SPEED_LS )
	{
		retval |= HUBF_Status_Low_Speed;
	}
	else if ( speed == XHCI_PS_SPEED_HS )
	{
		retval |= HUBF_Status_High_Speed;
	}

	// Change flags
	if ( status & XHCI_PS_CSC )
	{
		retval |= HUBF_Chg_Connection;
	}

	if ( status & XHCI_PS_PEC )
	{
		retval |= HUBF_Chg_Enable;
	}

	if ( status & XHCI_PS_OCC )
	{
		retval |= HUBF_Chg_Over_Current;
	}

	// Use software PortResetChange flag only (matching EHCI pattern).
	// Clear hardware PRC to prevent it from triggering spurious changes.
	if ( status & XHCI_PS_PRC )
	{
		U32 clr = status & ~XHCI_PS_CHANGE_MASK;
		clr |= XHCI_PS_PRC;
		PCI_WRITELONG( xhci->OpBase + XHCI_PORTSC( port - 1 ), clr );
	}

	if ( xhci->PortResetChange[ port ] )
	{
		retval |= HUBF_Chg_Reset;
	}

	TASK_NAME_LEAVE();

	return( retval );
}

// --
