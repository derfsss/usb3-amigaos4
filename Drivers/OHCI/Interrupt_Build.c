
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

SEC_CODE S32 OHCI_Interrupt_Build( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB2_EndPointNode *ep;
struct RealFunctionNode *fn;
struct OHCI_TD *dummy;
struct OHCI_TD *data;
struct OHCI_TD *last;
struct OHCI_ED *ed;
S32 handled;
S32 Zero;
S32 err;
U32 pid;
U32 val;
U32 len;
PTR buf;
U32 dt;
U32 l;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Interrupt_Build" );

	fn			= ioreq->req_Function;
	ep			= ioreq->req_EndPoint;

	handled		= TRUE;

	#if 1
//	#ifdef DO_DEBUG

	STR Speeds[] = { "Low","Full","High","Super" };

	USBINFO( "IORequest....... : %p",  ioreq );
	USBINFO( "CommandDir...... : %s",  ioreq->req_Public.io_Command == CMD_READ ? "Read" : "Write" );
	USBINFO( "SetupSize....... : %ld", ioreq->req_Public.io_SetupLength );
	USBINFO( "SetupData....... : %p",  ioreq->req_Public.io_SetupData );
	USBINFO( "Length.......... : %ld", ioreq->req_Public.io_Length );
	USBINFO( "Data............ : %p",  ioreq->req_Public.io_Data );
	USBINFO( "Address......... : %ld", fn->fkt_Address );
	USBINFO( "EndPointNr...... : %ld [Control]", ep->ep_Number );
	USBINFO( "MaxPacketSize... : %ld", ep->ep_MaxPacketSize );
	USBINFO( "Interval........ : %ld", ep->ep_Interval );
	USBINFO( "DataToggle...... : %ld", ep->ep_DataToggle );
	USBINFO( "HUB Port........ : %ld", fn->fkt_PortNr );
	USBINFO( "Fkt Speed....... : %ld [%s]", fn->fkt_Speed, Speeds[fn->fkt_Speed] );
	USBINFO( "Timeout......... : %ld%s", ioreq->req_Public.io_TimeOut, ( ioreq->req_Public.io_TimeOut == 0 ) ? " [Never]" : "" );
	USBINFO( "HCD Number...... : %ld", hn->hn_HCDIndex );
	USBINFO( "Ping State...... : %ld", ep->ep_PingState );

	#endif

	err		= USB2Err_Stack_NoMemory;
	len		= ( ioreq->req_Public.io_Length );
	buf		= ( ioreq->req_Public.io_Command == CMD_READ ) ? NULL : ioreq->req_Public.io_Data ;
	pid		= ( ioreq->req_Public.io_Command == CMD_READ ) ? OHCI_TD_IN : OHCI_TD_OUT;
	pid	   |=(( ioreq->req_Public.io_Command == CMD_READ ) && ( ioreq->req_Public.io_AllowShortPackets )) ? OHCI_TD_R : 0;
	dt		= ep->ep_DataToggle;

	Zero	=	(( ioreq->req_Public.io_Command == CMD_WRITE )
			&&	 ( ioreq->req_Public.io_AddZeroPackets )
			&&	 ( ioreq->req_Public.io_Length != 0 )
			&&	(( ioreq->req_Public.io_Length % ep->ep_MaxPacketSize ) == 0 )) ? TRUE : FALSE ;

	/* -- Header -- */

	ed = OHCI_Get_EDBuffer( hn, ioreq );

	if ( ! ed )
	{
		USBERROR( "Error allocating ED buffer" );
		err = USB2Err_Stack_NoMemory;
		goto bailout;
	}

	/* -- Data -- */

	last = NULL;

	do
	{
		data = OHCI_Get_TDBuffer( hn );

		if ( ! data )
		{
			USBERROR( "Error allocating TD buffer" );
			goto bailout;
		}

		if ( last )
		{
			last->td_nexttd		= LE_SWAP32( data->ptd_self_phy );
			last->ptd_virt_next	= data;
		}
		else
		{
			ed->ed_headp		= LE_SWAP32( data->ptd_self_phy );
			ed->ped_first_td	= data;
		}

		l = MIN( 2 * 4096, len );

		if ( ! OHCI_Get_8kBuffer( hn, data, buf, l ))
		{
			USBERROR( "Error allocating buffer" );
			goto bailout;
		}

		// -=-

		val  = pid;
		val |= OHCI_TD_NOCC;
		val |= OHCI_TD_NOINTR;
		val |= ( dt ) ? OHCI_TD_TOGGLE_1 : OHCI_TD_TOGGLE_0 ;

		data->td_flags = LE_SWAP32( val );

		// -=-

		last = data;

		if ( buf )
		{
			buf += l;
		}

		len -= l;

		dt = ( dt ) ? 0 : 1 ;
	}
	while( len );

	// ---

	if ( Zero )
	{
		data = OHCI_Get_TDBuffer( hn );

		if ( ! data )
		{
			USBERROR( "Error allocating TD buffer" );
			goto bailout;
		}

		last->td_nexttd		= LE_SWAP32( data->ptd_self_phy );
		last->ptd_virt_next	= data;
		last = data;
	}

	// ---

	val = LE_SWAP32( last->td_flags );

	val &= ~OHCI_TD_INTR_MASK;
	val |=  OHCI_TD_SET_DI( 1 );

	last->td_flags = LE_SWAP32( val );

	/* -- Dummy -- */

	dummy = OHCI_Get_TDBuffer( hn );

	if ( ! dummy )
	{
		USBERROR( "Error allocating TD buffer" );
		goto bailout;
	}

	last->td_nexttd		= LE_SWAP32( dummy->ptd_self_phy );
	last->ptd_virt_next	= dummy;

	/* Link TD into ED */
	ed->ed_tailp		= LE_SWAP32( dummy->ptd_self_phy );

	/* -- Done -- */

	err = USB2Err_NoError;

	handled	= FALSE;

bailout:

	if ( ! ioreq->req_Public.io_Error )
	{
		ioreq->req_Public.io_Error = err;
	}

	TASK_NAME_LEAVE();

	return( handled );
}

// --
