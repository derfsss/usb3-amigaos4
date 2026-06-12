
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

SEC_CODE S32 EHCI_Interrupt_Build( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB3_EndPointNode *ep;
//struct RealFunctionNode *fn;
struct EHCI_TD *last;
struct EHCI_TD *td;
struct EHCI_QH *qh;
U32 handled;
U32 Zero;
U32 err;
U32 len;
U32 pid;
U32 val;
U32 dt;
U32 l;
U8 *buf;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "EHCI : EHCI_Interrupt_Build" );

//	fn			= ioreq->req_Function;
	ep			= ioreq->req_EndPoint;
	dt 			= ep->ep_DataToggle;
	len			= ( ioreq->req_Public.io_Length );
	last		= NULL;
	handled		= TRUE;

	// --

	if ( ioreq->req_Public.io_Command == CMD_READ )
	{
		buf		= NULL;
		pid		= EHCI_TD_PID_IN;
		Zero	= FALSE;
	}
	else
	{
		buf		= ioreq->req_Public.io_Data;
		pid		= EHCI_TD_PID_OUT;
		Zero	=	(( ioreq->req_Public.io_AddZeroPackets )
				&&	 ( ioreq->req_Public.io_Length != 0 )
				&&	(( ioreq->req_Public.io_Length % ep->ep_MaxPacketSize ) == 0 )) ? TRUE : FALSE ;
	}

	// --

	#if 0
	#ifdef DO_DEBUG

	if ( ioreq->req_Public.io_Length >= 1024*20 )
	{
		STR Speeds[] = { "Low","Full","High","Super" };

		USBDEBUG( "IORequest....... : %p",  ioreq );
		USBDEBUG( "CommandDir...... : %s",  ioreq->req_Public.io_Command == CMD_READ ? "Read" : "Write" );
		USBDEBUG( "SetupSize....... : %ld", ioreq->req_Public.io_SetupLength );
		USBDEBUG( "SetupData....... : %p",  ioreq->req_Public.io_SetupData );
		USBDEBUG( "Length.......... : %ld", ioreq->req_Public.io_Length );
		USBDEBUG( "Data............ : %p",  ioreq->req_Public.io_Data );
		USBDEBUG( "Address......... : %ld", fn->fkt_Address );
		USBDEBUG( "EndPointNr...... : %ld [Interrupt]", ep->ep_Number );
		USBDEBUG( "MaxPacketSize... : %ld", ep->ep_MaxPacketSize );
		USBDEBUG( "Interval........ : %ld", ep->ep_Interval );
		USBDEBUG( "DataToggle...... : %ld", ep->ep_DataToggle );
		USBDEBUG( "HUB Port........ : %ld", fn->fkt_PortNr );
		USBDEBUG( "Fkt Speed....... : %ld [%s]", fn->fkt_Speed, Speeds[fn->fkt_Speed] );
		USBDEBUG( "Timeout......... : %ld%s", ioreq->req_Public.io_TimeOut, ( ioreq->req_Public.io_TimeOut == 0 ) ? " [Never]" : "" );
//		USBDEBUG( "HCD Name........ : %s",  hn->hn_HCDName );
		USBDEBUG( "HCD Number...... : %ld", hn->hn_HCDIndex );
//		USBDEBUG( "Ping State...... : %ld", ep->ep_PingState );
		USBERROR( "Add Zero Packet. : %ld", Zero );
	}

	#endif
	#endif

	// Check for bandwidth
	if ( ! EHCI_Enough_Bandwidth( hn, ioreq ))
	{
		USBDEBUG( "EHCI_Interrupt_Build : Error there is not Enough band width left for this request" );
		err = USB3Err_Host_NoBandWidth;
		goto bailout;
	}

	/* -- Header -- */

	qh = EHCI_Get_QHBuffer( hn, ioreq );

	if ( ! qh )
	{
		USBDEBUG( "EHCI_Interrupt_Build : Error allocating QH buffer" );
		err = USB3Err_Stack_NoMemory;
		goto bailout;
	}

	/* -- Data -- */

	do
	{
		td = EHCI_Get_TDBuffer( hn );

		if ( ! td )
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error Allocating Memory" );
			err = USB3Err_Stack_NoMemory;
			goto bailout;
		}

		if ( last )
		{
			last->td_td_next	= LE_SWAP32( td->ptd_self_phy );
			last->ptd_virt_next	= td;
		}
		else
		{
			qh->qh_td_next		= LE_SWAP32( td->ptd_self_phy );
			qh->pqh_first_td	= td;
		}

		l = MIN( 20480, len );

		if ( ! EHCI_Get_20kBuffer( hn, NULL, td, buf, l ))
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error allocating buffer" );
			err = USB3Err_Stack_NoMemory;
			goto bailout;
		}

		// --

		val	 = EHCI_TD_SET_PID( pid );
		val	|= EHCI_TD_SET_CERR( 3 );
		val |= EHCI_TD_SET_TOGGLE( dt );
		val |= EHCI_TD_SET_BYTES( l );
		val	|= EHCI_TD_ACTIVE;

		td->td_status = LE_SWAP32( val );

		// --

		last = td;

		if ( buf )
		{
			buf += l;
		}

		len -= l;

		// dt switch pr. Packet not pr. TD
//		dt = ( dt ) ? 0 : 1 ;
	}
	while( len );

	// --

	if ( Zero )
	{
		USBERROR( "Adding Interrupt Zero Packet" );

		td = EHCI_Get_TDBuffer( hn );

		if ( ! td )
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error allocating Memory" );
			err = USB3Err_Stack_NoMemory;
			goto bailout;
		}

		last->td_td_next	= LE_SWAP32( td->ptd_self_phy );
		last->ptd_virt_next	= td;

		if ( ! EHCI_Get_20kBuffer( hn, NULL, td, NULL, 0 ))
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error allocating buffer" );
			err = USB3Err_Stack_NoMemory;
			goto bailout;
		}

		// dt switch pr. Packet not pr. TD
//		dt = ( dt ) ? 0 : 1 ;

		val	 = EHCI_TD_SET_PID( pid );
		val	|= EHCI_TD_SET_CERR( 3 );
		val	|= EHCI_TD_SET_TOGGLE( dt );
		val	|= EHCI_TD_SET_BYTES( 0 );
		val	|= EHCI_TD_ACTIVE;

		td->td_status = LE_SWAP32( val );

		last = td;
	}

	// --
	// Last TD should trigger an interrupt

	if ( last )
	{
		last->td_status |= LE_SWAP32( EHCI_TD_IOC );
	}

	/* -- Done -- */

	#if 0
	static int qqqcnt=5;
	if ( ioreq->req_Public.io_Length >= 1024*20 && qqqcnt>0 )
	{
		qqqcnt--;
//		struct USBBase *usbbase = hn->hn_USBBase;
		EHCI_DUMP_QH( hn, qh, FALSE );
//		HCD_WAIT_MS( hn, 5 * 1000000 );
	}
	#endif

	err = USB3Err_NoError;

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


	#if 0

	handled	= TRUE;
	ep		= ioreq->req_EndPoint;
	err		= USB3Err_Stack_NoMemory;
	len		= ( ioreq->req_Public.io_Length );
	buf		= ( ioreq->req_Public.io_Command == CMD_READ ) ? NULL : ioreq->req_Public.io_Data ;
	pid		= ( ioreq->req_Public.io_Command == CMD_READ ) ? EHCI_TD_PID_IN : EHCI_TD_PID_OUT;
	dt 		= ep->ep_DataToggle;
	last	= NULL;

	Zero	=	(( ioreq->req_Public.io_Command == CMD_WRITE )
			&&	 ( ioreq->req_Public.io_AddZeroPackets )
			&&	 ( ioreq->req_Public.io_Length != 0 )
			&&	(( ioreq->req_Public.io_Length % ep->ep_MaxPacketSize ) == 0 )) ? TRUE : FALSE ;


	#if 0
	#ifdef DO_DEBUG

	STR Speeds[] = { "Low","Full","High","Super" };

	USBDEBUG( "IORequest....... %p",  ioreq );
	USBDEBUG( "CommandDir...... %s",  ioreq->req_Public.io_Command == CMD_READ ? "Read" : "Write" );
	USBDEBUG( "SetupSize....... %ld", ioreq->req_Public.io_SetupLength );
	USBDEBUG( "SetupData....... %p",  ioreq->req_Public.io_SetupData );
	USBDEBUG( "Length.......... %ld", ioreq->req_Public.io_Length );
	USBDEBUG( "Data............ %p",  ioreq->req_Public.io_Data );
	USBDEBUG( "Address......... %ld", fn->fkt_Address );
	USBDEBUG( "EndPointNr...... %ld [Interrupt]", ep->ep_Number );
	USBDEBUG( "MaxPacketSize... %ld", ep->ep_MaxPacketSize );
	USBDEBUG( "Interval........ %ld", ep->ep_Interval );
	USBDEBUG( "DataToggle...... %ld", ep->ep_DataToggle );
	USBDEBUG( "HUB Port........ %ld", fn->fkt_PortNr );
	USBDEBUG( "Fkt Speed....... %ld [%s]", fn->fkt_Speed, Speeds[fn->fkt_Speed] );
	USBDEBUG( "Timeout......... %ld%s", ioreq->req_Public.io_TimeOut, ( ioreq->req_Public.io_TimeOut == 0 ) ? " [Never]" : "" );
//	USBDEBUG( "HCD Name........ %s",  hn->hn_HCDName );
	USBDEBUG( "HCD Number...... %ld", hn->hn_HCDIndex );
//	USBDEBUG( "Ping State...... %ld", ep->ep_PingState );

	#endif
	#endif

	// Check for bandwidth
	if ( ! EHCI_Enough_Bandwidth( hn, ioreq ))
	{
		USBDEBUG( "EHCI_Interrupt_Build : Error there is not Enough band width left for this request" );
		err = USB3Err_Host_NoBandWidth;
		goto bailout;
	}

	/* -- Header -- */

	qh = EHCI_Get_QHBuffer( hn, ioreq );

	if ( ! qh )
	{
		USBDEBUG( "EHCI_Interrupt_Build : Error allocating QH buffer" );
		goto bailout;
	}

	/* -- Data -- */

	do
	{
		td = EHCI_Get_TDBuffer( hn );

		if ( ! td )
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error allocating Memory" );
			goto bailout;
		}

		if ( last )
		{
			last->td_td_next	= LE_SWAP32( td->ptd_self_phy );
			last->ptd_virt_next	= td;
		}
		else
		{
			qh->qh_td_next		= LE_SWAP32( td->ptd_self_phy );
			qh->pqh_first_td	= td;
		}

		l = MIN( 20480, len );

		if ( ! EHCI_Get_20kBuffer( hn, NULL, td, buf, l ))
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error allocating buffer" );
			goto bailout;
		}

		// --

		val	 = EHCI_TD_SET_PID( pid );
		val	|= EHCI_TD_SET_CERR( 3 );
		val |= EHCI_TD_SET_TOGGLE( dt );
		val |= EHCI_TD_SET_BYTES( l );
		val	|= EHCI_TD_ACTIVE;

		td->td_status = LE_SWAP32( val );

		// --
		
		last = td;

		buf += l;
		len -= l;

		// dt switch pr. Packet not pr. TD
//		dt = ( dt ) ? 0 : 1 ;
	}
	while( len );

	if ( Zero )
	{
		td = EHCI_Get_TDBuffer( hn );

		if ( ! td )
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error allocating Memory" );
			goto bailout;
		}

		last->td_td_next	= LE_SWAP32( td->ptd_self_phy );
		last->ptd_virt_next	= td;

		if ( ! EHCI_Get_20kBuffer( hn, NULL, td, NULL, 0 ))
		{
			USBDEBUG( "EHCI_Interrupt_Build : Error allocating buffer" );
			goto bailout;
		}

		// dt switch pr. Packet not pr. TD
//		dt = ( dt ) ? 0 : 1 ;

		val	 = EHCI_TD_SET_PID( pid );
		val	|= EHCI_TD_SET_CERR( 3 );
		val |= EHCI_TD_SET_TOGGLE( dt );
		val |= EHCI_TD_SET_BYTES( 0 );
		val	|= EHCI_TD_ACTIVE;

		td->td_status = LE_SWAP32( val );

		last = td;
	}

	if ( last )
	{
		last->td_status |= LE_SWAP32( EHCI_TD_IOC );
	}

	/* -- Done -- */

	err = USB3Err_NoError;
	handled	= FALSE;

bailout:

	if ( ! ioreq->req_Public.io_Error )
	{
		ioreq->req_Public.io_Error = err;
	}

	return( handled );
}
	#endif
