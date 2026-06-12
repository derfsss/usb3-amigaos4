
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
#if defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void EHCI_Dump_TD( struct USB2_HCDNode *hn, struct EHCI_TD *td )
{
struct ExecIFace *IExec;
STR txt;
U32 stat;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "EHCI : EHCI_Dump_TD" );

	IExec = hn->hn_USBBase->usb_IExec;

	IExec->DebugPrintF( "" );

	IExec->DebugPrintF( "  TD(%p) at 0x%08lx\n",  td, td->ptd_self_phy );

	txt = ( LE_SWAP32(td->td_td_next) & 0x01 ) ? "<T>" : "";

	IExec->DebugPrintF( "      td_td_next..: 0x%08lx %s\n", LE_SWAP32(td->td_td_next), txt );

	txt = ( LE_SWAP32(td->td_td_alt) & 0x01 ) ? "<T>" : "";

	IExec->DebugPrintF( "      td_td_alt...: 0x%08lx %s\n", LE_SWAP32(td->td_td_alt), txt );

	stat = LE_SWAP32(td->td_status);

	IExec->DebugPrintF( "      td_status...: 0x%08lx\n",
		stat
	);

	IExec->DebugPrintF( "                    toggle=%ld bytes=%ld ioc=%ld c_page=0x%lx\n",
		EHCI_TD_GET_TOGGLE(stat),
		EHCI_TD_GET_BYTES(stat),
		EHCI_TD_GET_IOC(stat),
		EHCI_TD_GET_C_PAGE(stat)
	);

	IExec->DebugPrintF( "                    cerr=%ld pid=%ld stat (%s%s%s%s%s%s%s%s)\n",
		EHCI_TD_GET_CERR(stat),
		EHCI_TD_GET_PID(stat),
		( stat & 0x80 ) ? "Active," : "",
		( stat & 0x40 ) ? "Halted," : "",
		( stat & 0x20 ) ? "Buferr," : "",
		( stat & 0x10 ) ? "Babble," : "",
		( stat & 0x08 ) ? "XActerr," : "",
		( stat & 0x04 ) ? "Missed," : "",
		( stat & 0x02 ) ? "Split," : "",
		( stat & 0x01 ) ? "Ping" : ""
	);

	IExec->DebugPrintF( "      td_buffer[0]: 0x%08lx\n", LE_SWAP32( td->td_buffer[0] ) );
	IExec->DebugPrintF( "      td_buffer[1]: 0x%08lx\n", LE_SWAP32( td->td_buffer[1] ) );
	IExec->DebugPrintF( "      td_buffer[2]: 0x%08lx\n", LE_SWAP32( td->td_buffer[2] ) );
	IExec->DebugPrintF( "      td_buffer[3]: 0x%08lx\n", LE_SWAP32( td->td_buffer[3] ) );
	IExec->DebugPrintF( "      td_buffer[4]: 0x%08lx\n", LE_SWAP32( td->td_buffer[4] ) );

	IExec->DebugPrintF( "      /* Private */\n" );

	IExec->DebugPrintF( "   ptd_self_phy   : 0x%08lx\n", td->ptd_self_phy );
	IExec->DebugPrintF( "   ptd_virt_next  : 0x%08lx\n", td->ptd_virt_next );
	IExec->DebugPrintF( "   ptd_buffer_phy : 0x%08lx\n", td->ptd_buffer_phy );
	IExec->DebugPrintF( "   ptd_buffer_virt: 0x%08lx\n", td->ptd_buffer_virt );

	IExec->DebugPrintF( "\n" );

	TASK_NAME_LEAVE();
}

#endif
// --
