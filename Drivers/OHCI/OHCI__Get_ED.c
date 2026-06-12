
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

SEC_CODE struct OHCI_ED *OHCI_Get_EDBuffer( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB2_EndPointNode *ep;
struct RealFunctionNode *fn;
struct OHCI_ED *ed;
U32 speed;
U32 phy;
U32 val;
U8 *mem;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Get_EDBuffer" );

	ed = NULL;

	// --

	if ( ioreq )
	{
		fn = ioreq->req_Function;
		ep = ioreq->req_EndPoint;

		switch( fn->fkt_Speed )
		{
			case USBSPEED_Low:
			{
				speed = OHCI_ED_SPEED;
				break;
			}

			case USBSPEED_Full:
			{
				speed = 0;
				break;
			}

			default:
			{
				#ifdef DO_DEBUG
				USBPANIC( "OHCI_Get_EDBuffer : Invalid Speed %lu", fn->fkt_Speed );
				#else
				USBDEBUG( "OHCI_Get_EDBuffer : Invalid Speed %lu", fn->fkt_Speed );
				#endif
				goto bailout;
			}
		}

		// Dest EndPoint Address
		val  = OHCI_ED_SET_FA( fn->fkt_Address );
		// Dest EndPoint Number
		val |= OHCI_ED_SET_EN( ep->ep_Number );
		// Get Direction from TD's
		val |= OHCI_ED_DIR_TD;
		// Speed, Low/Full
		val |= speed;
		// a General TD, ie non isochronous td
		val |= OHCI_ED_FORMAT_GEN;
		// Max Packet Size
		val |= OHCI_ED_SET_MAXP( ep->ep_MaxPacketSize );
	}
	else
	{
		val = LE_SWAP32( OHCI_ED_SKIP );
	}

	// --

	mem = MEMORY_ALLOC( MEMID_OHCI_ED, FALSE );

	if ( ! mem )
	{
		USBERROR( "OHCI_Get_EDBuffer : Error allocating memory" );
		goto bailout;
	}

	phy = ((struct Mem_FreeNode *)mem)->mfn_Addr;
	ed  = (struct OHCI_ED *)mem;

	/* Fill the ED */
	ed->ed_flags		= LE_SWAP32( val );
	ed->ed_tailp		= 0;
	ed->ed_headp		= 0;
	ed->ed_nexted		= 0;
	/* Private part */
	ed->ped_self_phy	= phy;
	ed->ped_virt_next	= NULL;
	ed->ped_first_td	= NULL;

	if ( ioreq )
	{
		ioreq->req_HCD.OHCI.ED_Header = ed;
	}

bailout:

	TASK_NAME_LEAVE();

	return(	ed );
}

// --
