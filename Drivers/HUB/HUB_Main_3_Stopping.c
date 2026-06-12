
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) at
 * Carlstedt Research & Technology.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// --

#include "usb3_all.h"

#include "HUB.h"

// --

SEC_CODE void HUB_Main__Stopping( struct USBBase *usbbase, struct intern *in )
{
U32 wait;
U32 mask;

	// -- Shutdown mode

	USBINFO( "__myMain (HUB)           : HUB_Main__Stopping (3)" );
	USBDEBUG( "FN Stopping Locks #%lu", in->Function->fkt_Locks );

	wait  = SIGBREAKF_CTRL_D;
	wait |=	in->Res_Interrupt->MsgPortBit;
	wait |=	in->Register->reg_Public.Stack_MsgPortBit;
	wait |= in->Tick_MsgPort.ump_Signal.sig_Signal_Mask;

	while( TRUE )
	{
		USBDEBUG( "HUB : Adr #%ld : FN    %p : Shutdown : Drivers: %ld, Functions: %ld, Header: %ld", 
			in->Function->fkt_Address, 
			in->Function,
			in->ASync_Drivers.ua_Counter, 
			in->ASync_Functions.ua_Counter, 
			in->FN_Header.uh_Nodes 
		);

		if (( in->FN_Header.uh_Nodes <= 0 )
		&&	( in->ASync_Drivers.ua_Counter <= 0 )
		&&	( in->ASync_Functions.ua_Counter <= 0 ))
		{
			break;
		}

		mask = TASK_WAIT( wait );

		if ( mask & in->Tick_MsgPort.ump_Signal.sig_Signal_Mask )
		{
//			USBDEBUG( "HUB : Shutdown : Got Timer Tick" );

			if ( MSGPORT_GETMSG( & in->Tick_MsgPort ))
			{
				HUB_Main__Free_FNs( usbbase, in );
				in->Tick_TimeRequest.Time.Seconds = 2;
				in->Tick_TimeRequest.Time.Microseconds = 0;
				IO_SEND( & in->Tick_TimeRequest );
			}
		}

		if ( mask &	in->Res_Interrupt->MsgPortBit )
		{
			USBDEBUG( "HUB : Shutdown : Got Int Bit" );

			HUB_Handle_Interrupt( usbbase, in );
		}

		if ( mask &	in->Register->reg_Public.Stack_MsgPortBit )
		{
			USBDEBUG( "HUB : Shutdown : Got Stack Bit" );

			HUB_Handle_Stack( usbbase, in );
		}

		if ( mask & SIGBREAKF_CTRL_D )
		{
			USBDEBUG( "HUB : Shutdown : Got CTRL+D" );

			HUB_Main__Free_FNs( usbbase, in );
		}
	}
}

// --
