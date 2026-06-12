
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

SEC_CODE void HUB__Port_FAdd_Retry( 
	struct RealFunctionNode *parent,
	struct USB2_PortStatus *stat,
	struct USBBase *usbbase, 
	struct intern *in, 
	U32 retry,
	U32 port )
{
U32 DevSpeed;
U32 delay;
S32 err;

	USBDEBUG( "HUB__Port_FAdd_Retry     : Set Port Feature( PORT_RESET )" );

	// --

	err = HUB__Port_Feature_Set( usbbase, in, port, PORT_RESET );

	if ( err == USB2Err_Device_OwnershipChange )
	{
		USBDEBUG( "HUB__Port_FAdd_Retry     : Ownership Change" );
		return;
	}

	if ( err != USB2Err_NoError )
	{
		USBDEBUG( "HUB__Port_FAdd_Retry     : Error Setting Port Reset (%ld)", err );
		return;
	}

	// --

	if ( in->Function->fkt_Tier == 0 )
	{
		// USB Specs 2.0 (7.1.7.5)
		// - Root hubs need 50ms minimum delay
		HUB_DELAY_MS( 100 );	// 100ms
	}
	else
	{
		HUB_DELAY_MS( 50 );		// 50ms
	}

	// --

	for( delay=0 ; delay<20 ; delay++ )
	{
		USBDEBUG( "HUB__Port_FAdd_Retry     : Get Port Feature( PORT_STATUS )" );

		err = HUB__Port_Status_Get( usbbase, in, port, stat );

		if ( err != USB2Err_NoError )
		{
			USBDEBUG( "HUB__Port_FAdd_Retry     : Error Getting Port Status (%ld)", err );
			return;
		}

		if ( stat->wPortChange & PORTCHGF_Reset )
		{
			USBDEBUG( "HUB__Port_FAdd_Retry     : Reset done, yay" );
			break;
		}

		HUB_DELAY_MS( retry );
	}

	// --

	USBDEBUG( "HUB__Port_FAdd_Retry     : Clr Port Feature( PORT_CLR_RESET )" );

	err = HUB__Port_Feature_Clr( usbbase, in, port, PORT_CLR_RESET );

	if ( err != USB2Err_NoError )
	{
		USBDEBUG( "HUB__Port_FAdd_Retry     : Error clearing Port Reset (%ld)", err );
		return;
	}

	// --

	USBDEBUG( "HUB__Port_FAdd_Retry     : Get Port Feature( PORT_STATUS )" );

	err = HUB__Port_Status_Get( usbbase, in, port, stat );

	if ( err != USB2Err_NoError )
	{
		USBDEBUG( "HUB__Port_FAdd_Retry     : Error Getting Port Status (%ld)", err );
		return;
	}

	if (( stat->wPortStatus & PORTSTATF_Connection ) == 0 )
	{
		USBDEBUG( "HUB__Port_FAdd_Retry     : No Device connected" );
		return;
	}

	// --

	/*
	** Figure out device speed.  This is a bit tricky because
	** UPS_PORT_POWER_SS and UPS_LOW_SPEED share the same bit.
	*/

	if (( stat->wPortStatus & PORTSTATF_Power ) == 0 )
	{
		stat->wPortStatus &= ~PORTSTATF_Power_SS;
	}

	/**/ if ( stat->wPortStatus & PORTSTATF_Super_Speed )
	{
		DevSpeed = USBSPEED_Super;
		USBDEBUG( "HUB__Port_FAdd_Retry     : Found a Super Speed Device" );
	}
	else if ( stat->wPortStatus & PORTSTATF_High_Speed )
	{
		DevSpeed = USBSPEED_High;
		USBDEBUG( "HUB__Port_FAdd_Retry     : Found a High Speed Device" );
	}
	else if ( stat->wPortStatus & PORTSTATF_Low_Speed )
	{
		DevSpeed = USBSPEED_Low;
		USBDEBUG( "HUB__Port_FAdd_Retry     : Found a Low Speed Device" );
	}
	else
	{
		/*
		** If there is no power bit set, it is certainly
		** a Super Speed device, so use the speed of its
		** parent hub.
		*/

		if ( stat->wPortStatus & PORTSTATF_Power )
		{
			DevSpeed = USBSPEED_Full;

			USBDEBUG( "HUB__Port_FAdd_Retry     : Found a Full Speed Device" );
		}
		else
		{
			DevSpeed = parent->fkt_Speed;

			USBDEBUG( "HUB__Port_FAdd_Retry     : Using Funktion Speed %ld", DevSpeed );
		}
	}

	/*
	** Reduce the speed, otherwise we won't setup the proper
	** transfer methods.
	*/

	if ( DevSpeed > parent->fkt_Speed )
	{
		USBDEBUG( "HUB__Port_FAdd_Retry     : Limiting Speed to %ld from %ld", parent->fkt_Speed, DevSpeed );

		DevSpeed = parent->fkt_Speed ;
	}

	HUB__Port_Function_Add3(
		parent,
		usbbase,
		in,
		DevSpeed,
		port
	);	
}

// --
