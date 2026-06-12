
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
#if defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void OHCI_Dump_TD( struct USB3_HCDNode *hn, struct OHCI_TD *td )
{
struct ExecIFace *IExec;
U32 nexttd;
U32 flags;
U32 cbp;
U32 be;

	if ( ! td )
	{
		goto bailout;
	}

	IExec = hn->hn_USBBase->usb_IExec;
	IExec->DebugPrintF( "\n" );

	IExec->DebugPrintF( "TD(%p) at 0x%08lx:\n", td, td->ptd_self_phy );

	flags = LE_SWAP32( td->td_flags );
	IExec->DebugPrintF( "  flags....: 0x%08lx\n", flags );

	IExec->DebugPrintF( "             CC=%lx EC=%lx T=%lx DI=%lx DP=%lx R=%lx\n",
		OHCI_TD_GET_CC(flags),
		OHCI_TD_GET_EC(flags),
		(( flags & OHCI_TD_TOGGLE_MASK ) >> 24 ),
		OHCI_TD_GET_DI(flags),
		(( flags & OHCI_TD_DP_MASK ) >> 19),
		(( flags & OHCI_TD_R ) >> 18) );

	cbp = LE_SWAP32( td->td_cbp );
	IExec->DebugPrintF( "  cbp......: 0x%08lx\n", cbp );

	nexttd = LE_SWAP32( td->td_nexttd );
	IExec->DebugPrintF( "  nexttd...: 0x%08lx\n", nexttd );

	be = LE_SWAP32( td->td_be );
	IExec->DebugPrintF( "  be.......: 0x%08lx\n", be );

	IExec->DebugPrintF( "     /* Private */\n" );

	IExec->DebugPrintF( "ptd_self_phy   : 0x%08lx\n", td->ptd_self_phy );
	IExec->DebugPrintF( "ptd_virt_next  : 0x%08lx\n", td->ptd_virt_next );
	IExec->DebugPrintF( "ptd_buffer_phy : 0x%08lx\n", td->ptd_buffer_phy );
	IExec->DebugPrintF( "ptd_buffer_virt: 0x%08lx\n", td->ptd_buffer_virt );
	IExec->DebugPrintF( "ptd_buffer_size: 0x%08lx\n", td->ptd_buffer_size );

	IExec->DebugPrintF( "\n" );

bailout:

	return;
}

#endif
// --
