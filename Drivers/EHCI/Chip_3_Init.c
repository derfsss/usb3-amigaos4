
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

/*
** Reverse a number with nbits bits.  Used to evenly distribute lower-level
** interrupt heads in the periodic schedule.
** Suitable for use with EHCI_IPOLLRATES <= 9.
*/

SEC_CODE U32 EHCI_Reverse_Bits( U32 c, U32 nbits )
{
//	#ifdef DO_DEBUG
//	if ( nbits > 8 )
//	{
//		struct USBBase *usbbase = hn->hn_USBBase;
//		USBPANIC( "EHCI_Reverse_Bits : nbits out of range (%lu)", nbits );
//	}
//	#endif

	c = (( c >> 1 ) & 0x55 ) | (( c << 1 ) & 0xaa );
	c = (( c >> 2 ) & 0x33 ) | (( c << 2 ) & 0xcc );
	c = (( c >> 4 ) & 0x0f ) | (( c << 4 ) & 0xf0 );

	return( c >> ( 8 - nbits ));
}

// --

SEC_CODE S32 EHCI_Chip_Init( struct USB3_HCDNode *hn )
{
//struct USBBase *usbbase;
struct EHCI_QH *qh;
S32 retval;
U32 slot;
U32 cnt;
U32 i;
U32 j;
U32 r;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "EHCI : EHCI_Chip_Init" );

//	usbbase = hn->hn_USBBase;

	retval = FALSE;

	// --

	USBDEBUG( "EHCI_Chip_Init           : 1 :" );
	USBDEBUG( "EHCI_Chip_Init           : 1 : ISlots %p", hn->hn_HCD.EHCI.FrameList_ISlots );

	/*
	 * Allocate the interrupt dummy QHs. These are arranged to give poll
	 * intervals that are powers of 2 times 1ms.
	 */

	for( cnt=0 ; cnt<255 ; cnt++ )
	{
		qh = EHCI_Get_QHBuffer( hn, NULL );

//		USBDEBUG( "EHCI_Chip_Init           : 1 : QH %p", qh );

		if ( ! qh )
		{
			USBDEBUG( "EHCI_Chip_Init           : Error allocating memory" );
			goto bailout;
		}

		hn->hn_HCD.EHCI.FrameList_ISlots[ cnt ] = qh;
	}

	// --

	USBDEBUG( "EHCI_Chip_Init           : 2 : Link Pyramide QH's" );

	/*
	** Link ms Pyramide
	**
	**    1ms                             o0
	**
	**    2ms                     o1              o2
	**
	**    4ms                 o3      o4      o5      o6
	**
	**    8ms               o7  o8  o9  10  11  12  13  14
	**
	**   16ms             15 ..                       ..  30
	**
	**   32ms           31 ..                           ..  62
	**
	**   64ms         63 ..                               ..  126
	**
	**  128ms      127 ..                                   ..  254
	*/

	for( cnt=0 ; cnt<255 ; cnt++ )
	{
		qh = hn->hn_HCD.EHCI.FrameList_ISlots[ cnt ];

		if ( ! cnt )
		{
			// The last (1ms) QH terminates.
			qh->pqh_virt_next	= NULL;
			qh->qh_link			= EHCI_NULL;
		}
		else
		{
			// Otherwise the next QH has half the poll interval.
			qh->pqh_virt_next	= hn->hn_HCD.EHCI.FrameList_ISlots[ ((( cnt + 1 ) / 2 ) - 1 ) ];
			qh->qh_link			= LE_SWAP32( qh->pqh_virt_next->pqh_self_phy | EHCI_LINK_QH );
		}
	}

	// --
	// -- Periodic Frame List
	// Point the frame list at the last level (128ms).

	USBDEBUG( "EHCI_Chip_Init           : 3 : Build Periodic Frame List" );

	for( i=0 ; i<128 ; i++ )
	{
		r = EHCI_Reverse_Bits( i, EHCI_IPOLLRATES - 1 );

		slot = EHCI_IQHIDX( EHCI_IPOLLRATES, r );

		qh = hn->hn_HCD.EHCI.FrameList_ISlots[ slot ];

		for( j=i ; j<1024 ; j += 128 )
		{
			hn->hn_HCD.EHCI.FrameList[j] = LE_SWAP32( qh->pqh_self_phy | EHCI_LINK_QH );
		}
	}

	// --
	// Allocate dummy QH that starts the async list.

	USBDEBUG( "EHCI_Chip_Init           : 4 : Alloc QH Dummy Header" );

	qh = EHCI_Get_QHBuffer( hn, NULL );

	if ( ! qh )
	{
		USBDEBUG( "Error allocating memory" );
		goto bailout;
	}

	// Fill the QH
	qh->qh_link			= LE_SWAP32( qh->pqh_self_phy | EHCI_LINK_QH );
	qh->qh_ep1			= LE_SWAP32( EHCI_QH_SET_EPS( EHCI_QH_SPEED_HIGH ) | ( EHCI_QH_HRECL ));
	qh->qh_ep2			= 0;
	qh->qh_td_cur		= 0;
	qh->qh_td_next		= EHCI_NULL;
	qh->qh_td_alt		= EHCI_NULL;
	qh->qh_status		= 0;	//LE_SWAP32( EHCI_TD_HALTED );
	qh->pqh_virt_next	= qh;

	hn->hn_HCD.EHCI.ASync_Head = qh;

	// --

	retval = TRUE;

bailout:

	USBDEBUG( "EHCI_Chip_Init           : 9 : retval %ld", retval );

	TASK_NAME_LEAVE();

	return( retval );
}

// --
