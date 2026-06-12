
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

SEC_CODE S32 EHCI_Chip_Stop( struct USB3_HCDNode *hn )
{
S32 retval;
U32 cnt;
U32 val;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "EHCI : EHCI_Chip_Stop" );

	retval = FALSE;

	USBDEBUG( "EHCI_Chip_Stop           : Halt Chip" );

	// --
	// Halt controller (if running)

	val = PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD );

	if ( val & EHCI_CMD_RS )
	{
		val &= ~EHCI_CMD_RS;
		PCI_WRITELONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBCMD, val );

		// --
		#define MAXMS	20		// max 5x20 = 100ms

		for( cnt=0 ; cnt<MAXMS ; cnt++ )
		{
			if ( PCI_READLONG( hn->hn_HCD.EHCI.CapLength + EHCI_USBSTS ) & EHCI_STS_HCH )
			{
				break;
			}
			else
			{
				HCD_WAIT_MS( hn, 5 );		// Wait 5ms
			}
		}

		if ( cnt == MAXMS )
		{
			USBDEBUG( "EHCI_Chip_Stop           : Error Stopping Controller" );
			goto bailout;
		}

		#undef MAXMS
		// --
	}

	// --

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
