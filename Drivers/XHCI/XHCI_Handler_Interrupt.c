
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

SEC_CODE U32 XHCI_Handler_Interrupt( struct ExceptionContext *Context UNUSED, struct ExecBase *SysBase UNUSED, PTR userData )
{
struct USB3_HCDNode *hn;
struct USBBase *usbbase;
struct ExecIFace *IExec;
struct _XHCI *xhci;
U32 status;
U32 iman;
U32 sigflags;

	hn      = (struct USB3_HCDNode *) userData;
	xhci    = & hn->hn_HCD.XHCI;
	usbbase = hn->hn_USBBase;
	IExec   = usbbase->usb_IExec;

	// Read USB Status
	status = hn->HCD_ReadLong( hn, xhci->OpBase + XHCI_USBSTS );

	// Check if this interrupt is for us
	if ( ! ( status & ( XHCI_STS_EINT | XHCI_STS_HSE | XHCI_STS_PCD ) ) )
	{
		return( 0 );	// Not our interrupt
	}

	// Acknowledge status bits (RW1C)
	hn->HCD_WriteLong( hn, xhci->OpBase + XHCI_USBSTS,
		status & ( XHCI_STS_EINT | XHCI_STS_HSE | XHCI_STS_PCD ) );

	// Clear interrupter pending
	iman = hn->HCD_ReadLong( hn, xhci->RunBase + XHCI_IMAN(0) );

	if ( iman & XHCI_IMAN_IP )
	{
		hn->HCD_WriteLong( hn, xhci->RunBase + XHCI_IMAN(0), iman | XHCI_IMAN_IP );
	}

	sigflags = 0;

	// Host System Error -- signal for chip restart
	if ( status & XHCI_STS_HSE )
	{
		sigflags |= hn->HCD_Restart_Chip.sig_Signal_Mask;
		IExec->DebugPrintF( "XHCI: Host System Error!\n" );
	}

	// Event interrupt or Port Change -- signal HCD task to process events
	if ( status & XHCI_STS_EINT )
	{
		sigflags |= xhci->Signal_Event.sig_Signal_Mask;
	}

	if ( status & XHCI_STS_PCD )
	{
		sigflags |= hn->hn_HUB_Signal.sig_Signal_Mask;
	}

	if ( sigflags )
	{
		TASK_SIGNAL( hn->hn_Task->tn_TaskAdr, sigflags );
	}

	return( 1 );
}

// --
