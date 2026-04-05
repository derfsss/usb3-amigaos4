
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

SEC_CODE struct EHCI_QH *EHCI_Get_QHBuffer( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB2_EndPointNode *ep;
struct RealFunctionNode *fn;
struct EHCI_QH *qh;
U32 speed;
U32 val1;
U32 val2;
U32 val3;
U32 phy;
U8 *mem;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "EHCI : EHCI_Get_QHBuffer" );

	qh = NULL;

	if ( ioreq )
	{
		fn = ioreq->req_Function;
		ep = ioreq->req_EndPoint;

		#ifdef DO_DEBUG

		if ( FUNCTION_VALID(fn) != VSTAT_Okay )
		{
			USBPANIC( "EHCI_Get_QHBuffer : FN    Null Pointer" );
		}

		if ( FUNCTION_VALID(fn->fkt_Parent) != VSTAT_Okay )
		{
			USBPANIC( "EHCI_Get_QHBuffer : fkt_Parent Null Pointer" );
		}

		if ( ENDPOINT_VALID(ep) != VSTAT_Okay )
		{
			USBPANIC( "EHCI_Get_QHBuffer : FN    Null Pointer" );
		}

		#endif

		// --

		switch( fn->fkt_Speed )
		{
			case USBSPEED_Low:
			{
				speed = EHCI_QH_SPEED_LOW;
				break;
			}

			case USBSPEED_Full:
			{
				speed = EHCI_QH_SPEED_FULL;
				break;
			}

			case USBSPEED_High:
			{
				speed = EHCI_QH_SPEED_HIGH;
				break;
			}

			default:
			{
				#ifdef DO_DEBUG
				USBPANIC( "EHCI_Get_QHBuffer : Invalid Speed %lu", fn->fkt_Speed );
				#else
				USBDEBUG( "EHCI_Get_QHBuffer : Invalid Speed %lu", fn->fkt_Speed );
				#endif
				goto bailout;
			}
		}

		// --

		val1 = 0;

		if ( ep->ep_Type == EPATT_Type_Control )
		{
			// Control EndPoint Flag. High Speed or Low/Full Speed
			val1 |= ( speed != EHCI_QH_SPEED_HIGH ) ? EHCI_QH_CTL : 0;
		}

		if ( ep->ep_Type != EPATT_Type_Interrupt )
		{
			// EHCI Specs - 4.9.x: Should not be set for Interupt QHs
			// Nak Count Reload
			val1 |= EHCI_QH_SET_NRL( 4 );
		}

		// Max Packet Size
		val1 |= EHCI_QH_SET_MPL( ep->ep_MaxPacketSize );

		// Data Toggle Control — use hardware toggle tracking for
		// non-control endpoints. Control endpoints use explicit
		// toggle values from TDs (Setup=0, Data=1, Status=1).
		if ( ep->ep_Type != EPATT_Type_Control )
		{
			val1 |= EHCI_QH_DTC;
		}

		// Speed 1.5/12/480Mbit
		val1 |= EHCI_QH_SET_EPS( speed );

		#ifdef DO_DEBUG

		if ( ep->ep_Number > 0x0f )
		{
			USBPANIC( "EHCI_Get_QHBuffer : Invalid EndPoint Number (%ld)", ep->ep_Number );
		}

		#endif
		
		// Dest EndPoint Number
		val1 |= EHCI_QH_SET_ENDPT( ep->ep_Number );

		#ifdef DO_DEBUG

		if ( fn->fkt_Address > 127 )
		{
			USBPANIC( "EHCI_Get_QHBuffer : Invalid Funktion Address #%lu", fn->fkt_Address );
		}
	
		#endif

		// Dest EndPoint Address
		val1 |= EHCI_QH_SET_ADDR( fn->fkt_Address );

		// --

		// High-Bandwidth Pipe Multiplier
		val2 = EHCI_QH_SET_MULT( 1 );
		
		// HUB Port Number
		// Only used by Host Controller if Low/Full Speed
		val2 |= EHCI_QH_SET_PORT( fn->fkt_Parent->fkt_PortNr );

		// HUB Address
		// Only used by Host Controller if Low/Full Speed
		val2 |= EHCI_QH_SET_HUBA( fn->fkt_Parent->fkt_Address );

		// Split Completion Mask
		// Referes to FrIndex number
		val2 |= EHCI_QH_SET_CMASK( 0x1c );

		if ( ep->ep_Type == EPATT_Type_Interrupt )
		{
			// Interrupt Schedule Mask
			val2 |= EHCI_QH_SET_SMASK( 0x01 );
		}

		// --

		val3 = 0;

		// --
	}
	else
	{
		val1 = EHCI_QH_SET_EPS( EHCI_QH_SPEED_HIGH );
		val2 = EHCI_QH_SET_MULT( 1 );
		val3 = EHCI_TD_HALTED;
	}

	// --

	mem = MEMORY_ALLOC( MEMID_EHCI_QH, FALSE );

	if ( ! mem )
	{
		USBERROR( "EHCI_Get_QHBuffer : Error allocating memory" );
		goto bailout;
	}

	phy = ((struct Mem_FreeNode *)mem)->mfn_Addr;
	qh  = (struct EHCI_QH *)mem;

	/* Fill the QH */
	qh->qh_link			= EHCI_NULL;
	qh->qh_ep1			= LE_SWAP32( val1 );
	qh->qh_ep2			= LE_SWAP32( val2 );
	qh->qh_td_cur		= 0;
	qh->qh_td_next		= EHCI_NULL;
	qh->qh_td_alt		= EHCI_NULL;
	qh->qh_status		= LE_SWAP32( val3 );
	qh->qh_buffer[0]	= 0;
	qh->qh_buffer[1]	= 0;
	qh->qh_buffer[2]	= 0;
	qh->qh_buffer[3]	= 0;
	qh->qh_buffer[4]	= 0;
	/* Private part */
	qh->pqh_self_phy	= phy;
	qh->pqh_virt_next	= NULL;
	qh->pqh_first_td	= NULL;
//	qh->qh_IOReq		= ioreq;		// Debug only

	if ( ioreq )
	{
		ioreq->req_HCD.EHCI.QH_Header = qh;
	}

	// --

bailout:

	TASK_NAME_LEAVE();

	return( qh );
}

// --
