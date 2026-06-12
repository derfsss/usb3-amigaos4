
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

SEC_CODE void OHCI_Bulk_Remove( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct OHCI_ED *last;
struct OHCI_ED *ed;
U32 val;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Bulk_Remove" );


	// -- Find our self

	ed = ioreq->req_HCD.OHCI.ED_Header;

	last = hn->hn_HCD.OHCI.Bulk_Head;

	while(( last ) && ( last->ped_virt_next != ed ))
	{
		last = last->ped_virt_next;
	}

	// -- Stop Control

	val = PCI_READLONG( OHCI_COMMAND_STATUS );

	if ( val & OHCI_BLF )
	{
		val &= ~OHCI_BLF;

		PCI_WRITELONG( OHCI_COMMAND_STATUS, val );

		while( PCI_READLONG( OHCI_COMMAND_STATUS ) & OHCI_BLF )
		{
			HCD_WAIT_MS( hn, 1 );		// Wait 1ms
		}
	}

	// -- Change Pointers so it Jump over ours
	
	last->ped_virt_next	= ed->ped_virt_next;
	last->ed_nexted		= ed->ed_nexted;

	// -- Restart Control

	val = PCI_READLONG( OHCI_COMMAND_STATUS );
	
	val |= OHCI_BLF;

	PCI_WRITELONG( OHCI_COMMAND_STATUS, val );
	
	// --
	// If it happens that oure ED is active,
	// we need to wait 2ms (3 to be sure).

	if (( LE_SWAP32( ed->ed_tailp ) & OHCI_HEADMASK )
	!=	( LE_SWAP32( ed->ed_headp ) & OHCI_HEADMASK ))
	{
		HCD_WAIT_MS( hn, 3 );		// Wait 3ms
	}

	// --
	// its no longer active, but still controlled by HCD
	ioreq->req_PublicStat = IORS_HCD;

	#if 0
	OHCI_DUMP_ED( hn, ed, TRUE );
	#endif

	TASK_NAME_LEAVE();
}

// --
