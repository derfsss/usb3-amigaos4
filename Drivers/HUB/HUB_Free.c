
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

SEC_CODE void HUB_Free( struct USBBase *usbbase, struct intern *in )
{
	USBINFO( "HUB : HUB_Free : 1" );

	// --

	if ( in->PortFunctions )
	{
		#ifdef DO_DEBUG
		for( U32 cnt=0 ; cnt<in->Ports ; cnt++ )
		{
			if ( in->PortFunctions[cnt] )
			{
				USBINFO( "HUB_Free                  : --> Port #%lu : Error Function %p (Still Around)", cnt, in->PortFunctions[cnt] );
				in->PortFunctions[cnt] = NULL;
			}
		}
		#endif

		MEM_FREEVEC( in->PortFunctions );
		in->PortFunctions = NULL;
	}

	// --

	USBINFO( "HUB : HUB_Free : 2" );

	if ( in->Register )
	{
		USBINFO( "HUB_Free                  : REG   %p : Unregister", in->Register );
		REGISTER_UNREGISTER( in->Register );
		in->Register = NULL;
	}

	// --
	// -- Tick Timer MsgPort

	USBINFO( "HUB : HUB_Free : 3" );

	if ( in->Tick_TimerAdded )
	{
		in->Tick_TimerAdded = FALSE;
		IO_STOP( & in->Tick_TimeRequest );
	}

	USBINFO( "HUB : HUB_Free : 4" );

	MSGPORT_FREE( & in->Tick_MsgPort );
	MSGPORT_FREE( & in->Delay_MsgPort );
	ASYNC_FREE( & in->ASync_Functions );
	ASYNC_FREE( & in->ASync_Drivers );

	USBINFO( "HUB : HUB_Free : 9" );
}

// --
