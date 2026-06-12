
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

SEC_CODE U32 OHCI_Slot_Find( struct USB3_HCDNode *hn, U32 interval )
{
U32 lowestslot;
U32 lowestbw;
U32 offset;
U32 slots;
U32 lev;
U32 cnt;
U32 pos;
U32 bw;
U32 l;

	// -- Find Level

	for( lev=5 ; lev ; lev-- )
	{
		if (( 1UL << lev ) <= interval )
		{
			break;
		}
	}

	// -- Find Slot

	slots		= 1UL << lev;
	lowestbw	= -1UL;
	lowestslot	= 0;
	offset		= ( 1UL << lev ) - 1;

	for( cnt=0 ; cnt<slots ; cnt++ )
	{
		bw = 0;

		pos = offset + cnt;

		for( l=0 ; l<=lev ; l++ )
		{
			bw += hn->hn_HCD.OHCI.BandWidth[ pos ];

			pos = ((( pos + 1 ) / 2 ) - 1 );
		}

		if ( bw < lowestbw )
		{
			lowestbw	= bw;
			lowestslot	= offset + cnt;

		}
	}

	// -- Adjust Slots

	pos = lowestslot;

	for( l=0 ; l<=lev ; l++ )
	{
		hn->hn_HCD.OHCI.BandWidth[ pos ]++;

		pos = ((( pos + 1 ) / 2 ) - 1 );
	}

	// --

	return( lowestslot );
}

// --
