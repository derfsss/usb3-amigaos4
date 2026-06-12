
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

SEC_CODE void OHCI_Interrupt_Remove( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct OHCI_ED *cur;
struct OHCI_ED *ed;
U32 head;
	
	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Interrupt_Remove" );

	ed = ioreq->req_HCD.OHCI.ED_Header;

	if ( ! ed )
	{
		USBERROR( "NULL Pointer" );
		goto bailout;
	}

	cur = ioreq->req_HCD.OHCI.Start_Header;

	while(( cur ) && ( cur->ped_virt_next != ed ))
	{
		cur = cur->ped_virt_next;
	}

	if ( ! cur )
	{
		USBERROR( "QH Header not found" );
		goto bailout;
	}

	OHCI_Slot_Free( hn, ioreq->req_HCD.OHCI.Start_Slot );

	// --
	cur->ped_virt_next	= ed->ped_virt_next;
	cur->ed_nexted		= ed->ed_nexted;

	// --
	// its no longer active, but still controlled by HCD
	ioreq->req_PublicStat = IORS_HCD;

	// --
	// If it happens that oure ED is active,
	// we need to wait 2ms (3 to be sure).
	// Give it time to update ToggleCarry

	HCD_WAIT_MS( hn, 3 );

	head = LE_SWAP32( ed->ed_headp );

	ioreq->req_EndPoint->ep_DataToggle = ( head & OHCI_TOGGLECARRY ) ? TRUE : FALSE;

	// --
bailout:

	TASK_NAME_LEAVE();
}

// --
