
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

#include "usb2_all.h"
#include "EHCI.h"

// --

SEC_CODE void EHCI_Control_Add( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct EHCI_QH *start;
struct EHCI_QH *last;
struct EHCI_QH *qh;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "EHCI : EHCI_Control_Add" );

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	EHCI_Door_Bell_Init( hn, __FILE__ );
	#else
	EHCI_Door_Bell_Init( hn );
	#endif

	// --
	// Find last QH and add it after

	start	= hn->hn_HCD.EHCI.ASync_Head;
	last	= start;

	while( last )
	{
		if ( last->pqh_virt_next == start )
		{
			break;
		}
		else
		{
			last = last->pqh_virt_next;
		}
	}

	// Should never happen

	if ( ! last )
	{
//		#ifdef DO_DEBUG
		hn->hn_USBBase->usb_IExec->DebugPrintF( "EHCI_Control_Add : Yikes NULL Pointer\n" );
//		#endif
		return;
	}

	// --
	// Link the new QH after 'last'
	qh = ioreq->req_HCD.EHCI.QH_Header;

	qh->pqh_virt_next		= last->pqh_virt_next;
	qh->qh_link				= last->qh_link;

	last->pqh_virt_next		= qh;
	last->qh_link			= LE_SWAP32( qh->pqh_self_phy | EHCI_LINK_QH );

	// --
	// Flush EHCI's async prefetch
	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	EHCI_Door_Bell_Wait( hn, __FILE__ );
	#else
	EHCI_Door_Bell_Wait( hn );
	#endif

	// --
	ioreq->req_PublicStat = IORS_HCD_Active;

	TASK_NAME_LEAVE();
}

// --
