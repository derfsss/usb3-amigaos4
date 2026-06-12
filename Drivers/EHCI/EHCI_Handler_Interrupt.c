
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
** Copyright (c) 2001 The NetBSD Foundation, Inc.
** All rights reserved.
**
** This code is derived from software contributed to The NetBSD Foundation
** by Lennart Augustsson (lennart@augustsson.net).
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. All advertising materials mentioning features or use of this software
**    must display the following acknowledgement:
**        This product includes software developed by the NetBSD
**        Foundation, Inc. and its contributors.
** 4. Neither the name of The NetBSD Foundation nor the names of its
**    contributors may be used to endorse or promote products derived
**    from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
** ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

// --

#include "usb3_all.h"
#include "EHCI.h"

// --

/*
** Called every time there is a HCD Interrupt to handle
*/

SEC_CODE U32 EHCI_Handler_Interrupt( struct ExceptionContext *Context UNUSED, struct ExecBase *SysBase UNUSED, PTR userData )
{
struct USB2_HCDNode *hn;
struct ExecIFace *IExec;
struct USBBase *usbbase;
U32 intflags;
U32 sigflags;

	TASK_NAME_ENTER( "EHCI : EHCI_Handler_Interrupt" );

	hn = (PTR) userData;

	// Validate..
	//  hn == NULL .. something is very wrong
	//  CapLength == 0 .. we have not been setup
	if (( ! hn ) || ( ! hn->hn_HCD.EHCI.CapLength ))
	{
		goto bailout;
	}

	usbbase = hn->hn_USBBase;

	IExec = usbbase->usb_IExec;
//	IExec->DebugPrintF( "EHCI Interrupt\n" );

	intflags = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBSTS );

	// Anything too handle
	if ( ! ( intflags & EHCI_STS_INTRS ))
	{
		goto bailout;
	}

	sigflags = 0;

	/* Clear the interrupt's cause */
	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBSTS, intflags );

	// RO : Host Controller Error
	if ( intflags & EHCI_STS_HSE )
	{
		// Do not need a Clear bit
		sigflags |= hn->HCD_Restart_Chip.sig_Signal_Mask;
		// -- Darn .. usb crash.. we need too restart by setting chip up again
		IExec->DebugPrintF( "\n##\n##\n## EHCI: EHCI_STS_HSE - Host System Error (Crash)\n##\n##\n\n" );
	}

	// RWC : DoorBell / Interrupt on async adv
	if ( intflags & EHCI_STS_IAA )
	{
		sigflags |= hn->hn_HCD.EHCI.Signal_IAA.sig_Signal_Mask;
//		IExec->DebugPrintF( "EHCI: EHCI_STS_IAA - Interrupt Async Advace\n" );
	}

	// RWC : Error Interrupt
	if ( intflags & EHCI_STS_ERRINT )
	{
		sigflags |= hn->hn_HCD.EHCI.Signal_EI.sig_Signal_Mask;
//		IExec->DebugPrintF( "EHCI: EHCI_STS_ERRINT - USB Error Interrupt\n" );
	}

	// RWC : Interrupt
	if ( intflags & EHCI_STS_INT )
	{
		sigflags |= hn->hn_HCD.EHCI.Signal_INT.sig_Signal_Mask;
//		IExec->DebugPrintF( "EHCI: EHCI_STS_INT - USB Interrupt\n" );
	}

	// RWC : Port Change Detect
	if ( intflags & EHCI_STS_PCD )
	{
		sigflags |= hn->hn_HUB_Signal.sig_Signal_Mask;
//		IExec->DebugPrintF( "EHCI: EHCI_STS_PCD - Port Change Detect\n" );
	}

	if ( sigflags )
	{
		TASK_SIGNAL( hn->hn_Task->tn_TaskAdr, sigflags );
//		IExec->DebugPrintF( "EHCI: Task %p, Signal %08lx\n", hn->hn_Task->tn_TaskAdr, sigflags );
	}

bailout:

	TASK_NAME_LEAVE();

	// Return Zero indicating Interupt was not handled, so
	// other Handlers that share our interupt line can have a go

	return( FALSE );
}

// --
