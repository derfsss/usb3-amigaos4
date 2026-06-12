
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
#include "OHCI.h"

// --

/*
** Called every time there is a HCD Interrupt to handle
*/

SEC_CODE U32 OHCI_Handler_Interrupt( struct ExceptionContext *Context UNUSED, struct ExecBase *SysBase UNUSED, PTR userData )
{
struct USB3_HCDNode *hn;
//struct ExecIFace *IExec;
struct USBBase *usbbase;
U32 intflags;
U32 sigflags;

	TASK_NAME_ENTER( "OHCI : OHCI_Handler_Interrupt" );

	hn		= (PTR) userData;
	usbbase = hn->hn_USBBase;
//	IExec	= usbbase->usb_IExec;

//	IExec->DebugPrintF( "OHCI Interrupt\n" );

	intflags = PCI_READLONG( OHCI_INTERRUPT_STATUS );
	sigflags = 0;

	// Anything too handle
	if ( ! intflags )
	{
		goto bailout;
	}

	/* Acknowledge */
	PCI_WRITELONG( OHCI_INTERRUPT_STATUS, intflags );

	/* Writeback Done Head */
	if ( intflags & OHCI_WDH )
	{
//		IExec->DebugPrintF( "OHCI: OHCI_WDH - Writeback Done Head\n" );
		intflags &= ~OHCI_WDH;
		sigflags |= hn->hn_HCD.OHCI.Signal_WDH.sig_Signal_Mask;
	}

	/* Root Hub Status Change */
	if ( intflags & OHCI_RHSC )
	{
//		IExec->DebugPrintF( "OHCI: OHCI_RHSC - Root Hub Status Change\n" );
		sigflags |= hn->hn_HUB_Signal.sig_Signal_Mask;
		intflags &= ~OHCI_RHSC;
	}

	/* Master Interrupt Enable */
	if ( intflags & OHCI_MIE )
	{
//		IExec->DebugPrintF( "OHCI: OHCI_MIE - Master Interrupt Enable\n" );
		intflags &= ~OHCI_MIE;
	}

	/* Start of Frame */
	if ( intflags & OHCI_SF )
	{
		// This is just a Info Flag, we don't use it 
		intflags &= ~OHCI_SF;
	}

	if ( intflags )
	{
//		IExec->DebugPrintF( "OHCI: Unprocessed flags 0x%08lx\n", intflags );
		/* Block unprocessed interrupts. */
		PCI_WRITELONG( OHCI_INTERRUPT_DISABLE, intflags );
	}

	if ( sigflags )
	{
//		IExec->DebugPrintF( "OHCI: Task %p, Signal %08lx\n", hn->hn_Task->tn_TaskAdr, sigflags );
		TASK_SIGNAL( hn->hn_Task->tn_TaskAdr, sigflags );
	}

bailout:

	TASK_NAME_LEAVE();

	// Return Zero indicating Interupt was not handled, so
	// other Handlers that share our interupt line can have a go

	return( FALSE );
}

// --
