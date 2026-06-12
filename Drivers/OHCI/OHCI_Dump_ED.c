
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

SEC_CODE void OHCI_Dump_ED( struct USB3_HCDNode *hn, struct OHCI_ED *ed, S32 DoSetup UNUSED )
{
struct ExecIFace *IExec;
U32 nexted;
U32 headp;
U32 flags;
U32 tailp;
STR txt1;
STR txt2;

	if ( ! ed )
	{
		goto bailout;
	}

	IExec = hn->hn_USBBase->usb_IExec;
	IExec->DebugPrintF( "\n" );

	IExec->DebugPrintF( "ED(%p) at 0x%08lx:\n", ed, ed->ped_self_phy );

	flags = LE_SWAP32( ed->ed_flags );
	IExec->DebugPrintF( "  flags....: 0x%08lx\n", flags );

	IExec->DebugPrintF( "             MPS=%ld Type=%lx Skip=%lx Spd=%lx Dir=%lx EPNr=%lx Adr=%lx\n",
		OHCI_ED_GET_MAXP(flags),
		(( flags & OHCI_ED_FORMAT_ISO ) >> 15 ),
		(( flags & OHCI_ED_SKIP ) >> 14 ),
		(( flags & OHCI_ED_SPEED ) >> 13 ),
		(( flags & OHCI_ED_DIR_MASK ) >> 11 ),
		OHCI_ED_GET_EN(flags),
		OHCI_ED_GET_FA(flags) );

	tailp = LE_SWAP32( ed->ed_tailp );
	IExec->DebugPrintF( "  tailp....: 0x%08lx\n", tailp );

	headp = LE_SWAP32( ed->ed_headp );
	txt1 = ( headp & OHCI_TOGGLECARRY ) ? "<Toggle>" : "";
	txt2 = ( headp & OHCI_HALTED ) ? "<Halt>" : "";
	IExec->DebugPrintF( "  headp....: 0x%08lx %s %s\n", headp, txt1, txt2 );

	nexted = LE_SWAP32( ed->ed_nexted );
	IExec->DebugPrintF( "  nexted...: 0x%08lx\n", nexted );

	IExec->DebugPrintF( " /* Private */\n" );

	IExec->DebugPrintF( " ped_self_phy: 0x%08lx\n", ed->ped_self_phy );
	IExec->DebugPrintF( "ped_virt_next: 0x%08lx\n", ed->ped_virt_next );
	IExec->DebugPrintF( " ped_first_td: 0x%08lx\n", ed->ped_first_td );

	IExec->DebugPrintF( "\n" );

	// if ( LE_SWAP32( ed->ed_flags ) & OHCI_ED_FORMAT_ISO )
	// {
	// 	struct ohci_soft_itd *sitd;

	// 	sitd = (struct ohci_soft_itd *)ed->ped_first_td;

	// 	while( sitd )
	// 	{
	// 		OHCI_dump_sitd( hn, sitd );

	// 		sitd = sitd->sitd_next;
	// 	}
	// }
	// else
	{
		struct OHCI_TD *td;

		td = ed->ped_first_td;

		while( td )
		{
			OHCI_Dump_TD( hn, td );

			td = td->ptd_virt_next;
		}
	}

bailout:

	return;
}

#endif
// --
