
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

SEC_CODE S32 HUB_Power_Up( struct USBBase *usbbase, struct intern *in )
{
S32 retval;
U32 delay;
U32 port;

	#ifdef DO_DEBUG
	struct USB3_PortStatus stat;
	#endif

	retval = FALSE;

	USBDEBUG( "HUB : HUB_Power_Up" );

	// --
	// Wait with power off for a while

//	HUB_DELAY_MS( 10 );

	// -- do Port Power On

	for( port=1 ; port <= in->Ports ; port++ )
	{
		// Lets add a Delay between each Port
		HUB_DELAY_MS( 10 );

		#ifdef DO_DEBUG
		// Get Stat.. but why?
		HUB__Port_Status_Get( usbbase, in, port, & stat );
		#endif

		// Turn the power on
		HUB__Port_Feature_Set( usbbase, in, port, PORT_POWER );

		#ifdef DO_DEBUG
		// Again Get Stat.. but why, for debug?
		HUB__Port_Status_Get( usbbase, in, port, & stat );
		#endif
	}

	// -- Do Port Power On Delay

	delay = in->Function->fkt_HUBDescriptor->PwrOn2PwrGood * 2;

	HUB_DELAY_MS( delay + USB_EXTRA_POWER_UP_TIME );

	// --

	return( retval );
}
