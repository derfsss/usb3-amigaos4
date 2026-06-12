
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

SEC_CODE S32 EHCI_Chip_Start( struct USB3_HCDNode *hn )
{
S32 retval;
U32 val;
U32 hch;
U32 cnt;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "EHCI : EHCI_Chip_Start" );

	retval = FALSE;

	// --

 	USBDEBUG( "EHCI_Chip_Start          : 1 : Set default 1024 frames" );

	val  = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD );
	val &= ~EHCI_CMD_FLS_MASK;
	val |=  EHCI_CMD_FLS_1024;
	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD, val );

	// hmm do I need a settle delay here? well why not
	HCD_WAIT_MS( hn, 2 );	// Wait 2ms

	// --
	// frame list size at default, read back what we got and use that

	val = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD );

	switch( val & EHCI_CMD_FLS_MASK )
	{
		case EHCI_CMD_FLS_1024:
		{
			hn->hn_HCD.EHCI.FrameList_Size = 1024;
			break;
		}

		case EHCI_CMD_FLS_512:
		{
			hn->hn_HCD.EHCI.FrameList_Size = 512;
			break;
		}

		case EHCI_CMD_FLS_256:
		{
			hn->hn_HCD.EHCI.FrameList_Size = 256;
			break;
		}

		case 3:
		{
			USBERROR( "EHCI_Chip_Start          : Error illegal FrameList size $%08lx", val );
			goto bailout;
		}
	}

	USBDEBUG( "EHCI_Chip_Start          : Frame List Size: %lu", hn->hn_HCD.EHCI.FrameList_Size );

	// --
	// Point to async list

	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_ASYNC, hn->hn_HCD.EHCI.ASync_Head->pqh_self_phy );
	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_PERIOD, hn->hn_HCD.EHCI.FrameList_Phyaddr );

	// --
	// Enable interrupts

	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBINTR, EHCI_INTR_NORMAL );

	// --
	// Turn on controller

	USBDEBUG( "EHCI_Chip_Start          : 5" );

	// Start controller : RS only for now
	val  = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD );
	val &= EHCI_CMD_FLS_MASK;	// Keep FrameList Size
	val |= EHCI_CMD_ITC_8;		// 8 microframes
	val |= EHCI_CMD_RS;
	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD, val );

	HCD_WAIT_MS( hn, 2 );	// Wait 2ms

	// Now enable periodic + async schedules
	val |= EHCI_CMD_ASE;
	val |= EHCI_CMD_PSE;
	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD, val );

	// --
	// Take over port ownership
	#define MAXMS	20		// max 5x20 = 100ms

	USBDEBUG( "EHCI_Chip_Start          : 6 :" );
	PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_CONFIGFLAG, EHCI_CONF_CF );

	for( cnt=0 ; cnt<MAXMS ; cnt++ )
	{
		hch = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBSTS ) & EHCI_STS_HCH;

		if ( ! hch )
		{
			break;
		}
		else
		{
			HCD_WAIT_MS( hn, 5 );	// Wait 5ms
		}
	}

	if ( cnt == MAXMS )
	{
		USBERROR( "EHCI_Chip_Start          : Taking over Port timed out" );
		goto bailout;
	}

	#undef MAXMS
	// --

	retval = TRUE;

bailout:

	#if 0
{
	val  = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD );
	USBINFO( "USBCMD: $%08lx", (U32) val );
	val  = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBSTS );
	USBINFO( "USBSTS: $%08lx", (U32) val );
	val  = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_ASYNC );
	USBINFO( "ASYNC : $%08lx", (U32) val );
	val  = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_PERIOD );
	USBINFO( "PERIOD: $%08lx", (U32) val );
}
	#endif

	TASK_NAME_LEAVE();

	return( retval );
}

// --
