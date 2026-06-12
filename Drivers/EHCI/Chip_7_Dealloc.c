
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

SEC_CODE void EHCI_Chip_Dealloc( struct USB2_HCDNode *hn )
{
	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "EHCI : EHCI_Chip_Dealloc" );

	// --

	USBDEBUG( "EHCI_Chip_Dealloc : 1 :" );

	if ( hn->hn_HCD.EHCI.PortResetChange )
	{
		MEM_FREEVEC( hn->hn_HCD.EHCI.PortResetChange );
		hn->hn_HCD.EHCI.PortResetChange = NULL;
	}

	// --

	USBDEBUG( "EHCI_Chip_Dealloc : 2 :" );

	if ( hn->hn_HCD.EHCI.FrameList_ISlots )
	{
		MEM_FREEVEC( hn->hn_HCD.EHCI.FrameList_ISlots );
		hn->hn_HCD.EHCI.FrameList_ISlots = NULL;
	}

	// --

	USBDEBUG( "EHCI_Chip_Dealloc : 3 : Free FrameList" );

	if ( hn->hn_HCD.EHCI.FrameList )
	{
		MEMORY_FREE( MEMID_HCD_4k, hn->hn_HCD.EHCI.FrameList, hn->hn_HCD.EHCI.FrameList_Phyaddr );
		hn->hn_HCD.EHCI.FrameList = NULL;
	}

	// --

	USBDEBUG( "EHCI_Chip_Dealloc : 4 :" );

	TASK_FREESIGNAL( & hn->hn_HCD.EHCI.Signal_IAA );
	TASK_FREESIGNAL( & hn->hn_HCD.EHCI.Signal_INT );
	TASK_FREESIGNAL( & hn->hn_HCD.EHCI.Signal_EI );
	hn->HCD_Mask = 0;

	// --

	TASK_NAME_LEAVE();
}

// --
