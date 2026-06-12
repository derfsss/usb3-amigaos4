
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

SEC_CODE struct RealFunctionNode *HUB__Port_Function_Add3( 
	struct RealFunctionNode *parent,
	struct USBBase *usbbase, 
	struct intern *in,
	U32 speed,
	U32 port )
{
struct RealFunctionNode *fn;
enum TaskReturn stat;
S32 error;
U32 adr;

	#if 0
	usbbase->usb_IExec->DebugPrintF(
		"\n"
		"##\n"
		"## HUB : HUB__Port_Function_Add3 : Port #%lu\n"
		"##\n"
		"\n", port
	);
	#endif

	error = TRUE;

	fn = FUNCTION_ALLOC( in->HCDNode, & in->ASync_Functions, speed, in->Function->fkt_Tier+1 );

	if ( ! fn )
	{
		USBDEBUG( "HUB__Port_Function_Add : Failed to alloc FN" );
		goto bailout;
	}

	NODE_ADDTAIL( & in->FN_Header, & fn->fkt_DriverNode );

	fn->fkt_PortNr	= port;
	fn->fkt_Parent	= parent;

	// --

	adr = HCD_ADDR_OBTAIN( in->HCDNode, fn );

	if ( ! adr )
	{
		USBDEBUG( "HUB__Port_Function_Add : Failed to get Function Address" );
		goto bailout;
	}

	fn->fkt_Address = adr;

	if ( ! HUB__Port_Function_SetAddr( usbbase, in, fn ))
	{
		USBDEBUG( "HUB__Port_Function_Add : Failed to set Function Address" );
		goto bailout;
	}

	stat = FUNCTION_INIT( fn, & in->ASync_Drivers );

	if ( ! stat )
	{
		USBDEBUG( "HUB__Port_Function_Add : Function Init failed : FN    %p : Stat $%08lx", fn, (U32) stat );
		goto bailout;
	}

	if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
	{
		USBDEBUG( "555 OKAY 444 : FN    %p", fn );

		fn->fkt_HUB_Data = in->HUB_ID;
		in->PortFunctions[port] = fn;
		error = FALSE;

		// Try and Bind Function to a Driver
		// We don't care if there is a driver for it
		FUNCTION_BIND( fn, & in->ASync_Drivers );
	}
	else
	{
		USBDEBUG( "HUB__Port_Function_Add : Failed to Lock Function : FN    %p", fn );
	}

bailout:

	if (( fn ) && ( error ))
	{
		NODE_REMNODE( & in->FN_Header, & fn->fkt_DriverNode );
		FUNCTION_FREE( fn );
		fn = NULL;
	}

	return( fn );
}

// --
