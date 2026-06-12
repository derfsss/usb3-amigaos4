
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

SEC_CODE void OHCI_Control_Add( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct OHCI_ED *last;
struct OHCI_ED *new;
U32 val;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Control_Add" );

	// -- Find Last Entry

	last = hn->hn_HCD.OHCI.Ctrl_Head;

	while(( last ) && ( last->ped_virt_next ))
	{
		last = last->ped_virt_next;
	}

	// -- Stop Control

	val = PCI_READLONG( OHCI_COMMAND_STATUS );

	if ( val & OHCI_CLF )
	{
		val &= ~OHCI_CLF;

		PCI_WRITELONG( OHCI_COMMAND_STATUS, val );

		while( PCI_READLONG( OHCI_COMMAND_STATUS ) & OHCI_CLF )
		{
			HCD_WAIT_MS( hn, 1 );		// Wait 1ms
		}
	}

	// -- Link it
	new = ioreq->req_HCD.OHCI.ED_Header;

	last->ed_nexted = LE_SWAP32( new->ped_self_phy );
	last->ped_virt_next = new;

	// -- Restart Control

	val = PCI_READLONG( OHCI_COMMAND_STATUS );

	val |= OHCI_CLF;

	PCI_WRITELONG( OHCI_COMMAND_STATUS, val );

	// --
	ioreq->req_PublicStat = IORS_HCD_Active;

	TASK_NAME_LEAVE();
}

// --
