
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

SEC_CODE void HUB_Main__Free_FNs( struct USBBase *usbbase, struct intern *in )
{
struct RealFunctionNode *fn;
struct USB3_Node *next;
struct USB3_Node *n;

	n = in->FN_Header.uh_Head;

	while( n )
	{
		next = Node_Next( n );
		fn = n->un_Data;
		// --

		if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ) || ( ! in->Running ))
		{
			USBDEBUG( "HUB_Main__Free_FNs                :  1 : FN    %p : Locks %2ld", fn, fn->fkt_Locks );

			// -- Release own lock

			if ( fn->fkt_HUB_Data == in->HUB_ID )
			{
				USBINFO( "HUB_Main__Free_FNs                :  2 : FN    %p : Release own HUB lock", fn );

				in->PortFunctions[ fn->fkt_PortNr ] = NULL;
				fn->fkt_HUB_Data = 0;
				FUNCTION_UNLOCK( fn );
			}

			// -- Try and Free Function

			#if 1
			NODE_REMNODE( & in->FN_Header, & fn->fkt_DriverNode );

			if ( FUNCTION_FREE( fn ) != FSTAT_Okay )
			{
				NODE_ADDHEAD( & in->FN_Header, & fn->fkt_DriverNode );

				USBINFO( "HUB_Main__Free_FNs                :  3 : FN    %p : Locks %2ld : Failed to Free", fn, fn->fkt_Locks );
			}
			else
			#endif
			{
				USBDEBUG( "HUB_Main__Free_FNs                :  4 : FN    %p : Free'd", fn );
			}
		}

		// --
		n = next;	
	}
}

// --

SEC_CODE void HUB_Main( struct USBBase *usbbase, struct intern *in )
{
	USBINFO( "__myMain (HUB)           : Enter" );

	// --
	// Normal running loop
	HUB_Main__Normal(	usbbase, in );

	// --
	// Signal subtasks to shutdown
	HUB_Main__Signal( 	usbbase, in );

	// --
	// Stopping loop, wait for Resources to be released
	HUB_Main__Stopping( usbbase, in );

	// --
	// Start shutdown
	HUB_Main__Shutdown( usbbase, in );

	// --
	// -- Exit

	USBINFO( "__myMain (HUB)           : Leave" );
	return;
}

// --
