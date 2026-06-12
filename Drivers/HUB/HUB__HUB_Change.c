
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

SEC_CODE void HUB__HUB_Chg( struct USBBase *usbbase UNUSED, struct intern *in UNUSED )
{
	// --

//	usbbase->usb_IExec->Disable();
//	usbbase->usb_IExec->DebugPrintF( "\nHUB : Handle HUB Change\n" );
//	usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//	usbbase->usb_IExec->Enable();

	// --

	#if 0
struct USB3_HUBStatus stat;
S32 err;

//	  usbbase->usb_IExec->Disable();
//	  usbbase->usb_IExec->DebugPrintF( "\nHandle HUB Change\n" );
//	  usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//	  usbbase->usb_IExec->Enable();

	err = HUB_GetHUBStatus( usbbase, in, &stat );

	if ( err != USB3Err_NoError )
	{
		USBDEBUG( "Error reading HUB Status (%ld)", err );
		goto bailout;
	}

	if ( stat.wHubChange & HUBCHGF_Local_Power_Chg )
	{
		USBDEBUG( "Clearing Local HUB Power Change" );

		HUB_ClrHUBFeature( usbbase, in, C_HUB_LOCAL_POWER );

		if ( stat.wHubStatus & HUBSTATF_Local_Power )
		{

		}
	}

	if ( stat.wHubChange & HUBCHGF_Over_Current_Chg )
	{
		USBDEBUG( "Clearing HUB Current Change" );

		HUB_ClrHUBFeature( usbbase, in, C_HUB_OVER_CURRENT );

		if ( stat.wHubStatus & HUBSTATF_Local_Power )
		{

		}
	}

bailout:
	#endif

	return;
}
