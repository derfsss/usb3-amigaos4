
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

SEC_CODE struct EHCI_TD *EHCI_Get_TDBuffer( struct USB3_HCDNode *hn )
{
struct EHCI_TD *td;
U32 phy;
U8 *mem;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "EHCI : EHCI_Get_TDBuffer" );

	// --

	td = NULL;
	mem = MEMORY_ALLOC( MEMID_EHCI_TD, FALSE );

	if ( ! mem )
	{
		USBERROR( "EHCI_Get_TDBuffer : Error allocating memory" );
		goto bailout;
	}

	phy = ((struct Mem_FreeNode *)mem)->mfn_Addr;
	td  = (struct EHCI_TD *)mem;

	// --
	// Fill the QTD
	td->td_td_next			= EHCI_NULL;
	td->td_td_alt			= EHCI_NULL;
	td->td_status			= 0;
	td->td_buffer[0]		= 0;
	td->td_buffer[1]		= 0;
	td->td_buffer[2]		= 0;
	td->td_buffer[3]		= 0;
	td->td_buffer[4]		= 0;
	// Private - Software
	td->ptd_self_phy		= phy;
	td->ptd_virt_next		= NULL;
	td->ptd_buffer_phy		= 0;
	td->ptd_buffer_virt		= NULL;
	td->ptd_buffer_size		= 0;

bailout:

	TASK_NAME_LEAVE();

	return( td );
}

// --
