
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

SEC_CODE void HUB__Port_Function_Rem( struct USBBase *usbbase, struct intern *in, U32 port )
{
struct RealFunctionNode *fn;

	#ifdef DO_STABLE

	if (( port == 0 ) || ( port > in->Ports ))
	{
		USBDEBUG( "HUB__Port_Function_Rem : Illegal Port Number (%lu)", port );
		goto bailout;
	}

	if ( ! in->PortFunctions[port] )
	{
		USBDEBUG( "HUB__Port_Function_Rem : Warning Port %lu empty", port );
		goto bailout;
	}

	#endif

	fn = in->PortFunctions[port];

	if (( fn ) && ( fn->fkt_HUB_Data == in->HUB_ID ))
	{
		in->PortFunctions[port] = NULL;
		fn->fkt_HUB_Data = 0;
		FUNCTION_UNLOCK( fn );

		USBDEBUG( "HUB__Port_Function_Rem      : FN    %p : HUB_ID $%08lx : Port #%lu", fn, in->HUB_ID, port );

		// Mark detached first
		FUNCTION_MARKDETACH( fn );

		// Send Stop signal to any driver 
		TASK_STOP_FN( fn );
	}
	else
	{
		USBPANIC( "What not our ID" );
	}

	#ifdef DO_STABLE
	bailout:
	#endif

	return;
}

// --
