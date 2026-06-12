
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

SEC_CODE S32 EHCI_Control_Build( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
//struct USB2_EndPointNode *ep;
//struct RealFunctionNode *fn;
struct EHCI_TD *status;
struct EHCI_TD *setup;
struct EHCI_TD *data;
struct EHCI_TD *last;
struct EHCI_QH *qh;
S32 handled;
S32 err;
U32 pid;
U32 val;
U32 len;
PTR buf;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "EHCI : EHCI_Control_Build" );
//	hn->hn_USBBase->usb_IExec->DebugPrintF( "EHCI_Control_Build : IOReq %p\n", ioreq );

//	fn			= ioreq->req_Function;
//	ep			= ioreq->req_EndPoint;

	handled		= TRUE;

	#if 0
	#ifdef USBINFO

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
//	USBINFO( "DataToggle...... : %ld", ep->ep_DataToggle );
	USBINFO( "HUB Port........ : %ld", fn->fkt_PortNr );
	USBINFO( "Fkt Speed....... : %ld [%s]", fn->fkt_Speed, Speeds[fn->fkt_Speed] );
	USBINFO( "Timeout......... : %ld%s", ioreq->req_Public.io_TimeOut, ( ioreq->req_Public.io_TimeOut == 0 ) ? " [Never]" : "" );
//	USBINFO( "HCD Name........ : %s",  hn->hn_HCDName );
	USBINFO( "HCD Number...... : %ld", hn->hn_HCDIndex );
//	USBINFO( "Ping State...... : %ld", ep->ep_PingState );
  
	#endif
	#endif

	/* -- Header -- */

	qh = EHCI_Get_QHBuffer( hn, ioreq );

	if ( ! qh )
	{
		USBDEBUG( "EHCI_Control_Build : Error allocating QH buffer" );
		err = USB2Err_Stack_NoMemory;
		goto bailout;
	}

	/* -- Setup -- */

	setup = EHCI_Get_TDBuffer( hn );

	if ( ! setup )
	{
		USBDEBUG( "EHCI_Control_Build : 1 : Error allocating TD buffer" );
		err = USB2Err_Stack_NoMemory;
		goto bailout;
	}

	qh->qh_td_next		= LE_SWAP32( setup->ptd_self_phy );
	qh->pqh_first_td	= setup;

	// Control always start with DataToggle off
	// Setup TD must not have Ping

	len  = ioreq->req_Public.io_SetupLength;
	buf	 = ioreq->req_Public.io_SetupData;

	val  = EHCI_TD_SET_TOGGLE( 0 );
	val |= EHCI_TD_SET_CERR( 3 );
	val |= EHCI_TD_SET_PID( EHCI_TD_PID_SETUP );
	val |= EHCI_TD_SET_BYTES( len );
	val |= EHCI_TD_ACTIVE;

	setup->td_status	= LE_SWAP32( val );

	if (( len ) && ( buf ))
	{
		if ( ! EHCI_Get_20kBuffer( hn, NULL, setup, buf, len ))
		{
			USBDEBUG( "EHCI_Control_Build : 2 : Error allocating 20k buffer" );
			err = USB2Err_Stack_NoMemory;
			goto bailout;
		}
	}

	last = setup;

	// -- Need it for AltNext

	status = EHCI_Get_TDBuffer( hn );

	if ( ! status )
	{
		USBDEBUG( "EHCI_Control_Build : 3 : Error allocating TD buffer" );
		err = USB2Err_Stack_NoMemory;
		goto bailout;
	}

	/* -- Data -- */
	// TD chaining loop: supports data phases larger than 20KB
	// by splitting into multiple TDs, each handling up to 20480 bytes.

	if ( ioreq->req_Public.io_Length )
	{
		U32 l;

		len		 = ioreq->req_Public.io_Length;
		pid		 = ( ioreq->req_Public.io_Command == CMD_READ ) ? EHCI_TD_PID_IN : EHCI_TD_PID_OUT;
		buf		 = ( ioreq->req_Public.io_Command == CMD_READ ) ? NULL : ioreq->req_Public.io_Data;

		do
		{
			data = EHCI_Get_TDBuffer( hn );

			if ( ! data )
			{
				USBDEBUG( "EHCI_Control_Build : 4 : Error Allocating Memory" );
				last->ptd_virt_next = status;
				err = USB2Err_Stack_NoMemory;
				goto bailout;
			}

			last->td_td_next	= LE_SWAP32( data->ptd_self_phy );
			last->ptd_virt_next	= data;
			last = data;

			l = MIN( 20480, len );

			val		 = EHCI_TD_SET_PID( pid );
			val		|= EHCI_TD_SET_CERR( 3 );
			val		|= EHCI_TD_SET_TOGGLE( 1 );
			val		|= EHCI_TD_SET_BYTES( l );
			val		|= EHCI_TD_ACTIVE;

			data->td_status		= LE_SWAP32( val );
			data->td_td_alt		= LE_SWAP32( status->ptd_self_phy );

			if ( ! EHCI_Get_20kBuffer( hn, NULL, data, buf, l ))
			{
				USBDEBUG( "EHCI_Control_Build : Error allocating buffer" );
				last->ptd_virt_next = status;
				err = USB2Err_Stack_NoMemory;
				goto bailout;
			}

			if ( buf )
			{
				buf += l;
			}

			len -= l;
		}
		while( len );
	}

	/* -- Status -- */

	last->td_td_next = LE_SWAP32( status->ptd_self_phy );
	last->ptd_virt_next = status;

	// As I understand it, it always ends with DataToggle on
	// Status can have Ping for out going TD

	val  = EHCI_TD_SET_TOGGLE( 1 );
	val |= EHCI_TD_SET_BYTES( 0 );
	val |= EHCI_TD_SET_CERR( 3 );
	val |= EHCI_TD_SET_PID( ioreq->req_Public.io_Command == CMD_READ ? EHCI_TD_PID_OUT : EHCI_TD_PID_IN );
	val |= EHCI_TD_ACTIVE;
	val |= EHCI_TD_IOC;

	status->td_status = LE_SWAP32( val );

	// --
	/* -- Done -- */

	#if 0
	EHCI_DUMP_QH( hn, qh, TRUE );
	#endif

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
