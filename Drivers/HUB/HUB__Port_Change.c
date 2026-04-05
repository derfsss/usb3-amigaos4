
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

#include "usb2_all.h"

#include "HUB.h"

// --

SEC_CODE void HUB__Port_Chg( struct USBBase *usbbase, struct intern *in, U32 port )
{
struct USB2_PortStatus stat;
S32 err;

	// --

	// --

	#ifdef DO_STABLE

	if (( port == 0 ) || ( port > in->Ports ))
	{
		USBERROR( "HUB__Port_Chg : Illegal Port Number (%lu)", port );
		goto bailout;
	}

	#endif

	// -- Get Status

	USBDEBUG( "HUB__Port_Chg : Handle Port Change for Port %lu", port );

	err = HUB__Port_Status_Get( usbbase, in, port, & stat );

	if ( err != USB2Err_NoError )
	{
		USBERROR( "HUB__Port_Chg : Error reading Port Status (%ld)", err );
		goto bailout;
	}

	usbbase->usb_IExec->DebugPrintF( "USB2: Port_Chg: port=%ld status=0x%04lx change=0x%04lx err=%ld\n",
		port, (U32) stat.wPortStatus, (U32) stat.wPortChange, err );

	// -- Handle Port Change

	if ( stat.wPortChange & PORTCHGF_Connection )
	{
		#ifdef DO_INFO
		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "\nHUB : Clearing Connect Change\n" );
		usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
		usbbase->usb_IExec->DebugPrintF( "Port        : %ld\n", port );
		usbbase->usb_IExec->Enable();
		#endif

		err = HUB__Port_Feature_Clr( usbbase, in, port, PORT_CLR_CONNECTION );

		if ( err != USB2Err_NoError )
		{
			USBERROR( "HUB__Port_Chg : Error clearing Port Connect (%ld)", err );
			goto bailout;
		}

		if ( in->PortFunctions[port] )
		{
			#ifdef DO_INFO
			usbbase->usb_IExec->Disable();
			usbbase->usb_IExec->DebugPrintF( "\nRemoving Device\n" );
			usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
			usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
			usbbase->usb_IExec->Enable();
			#endif

			struct RealFunctionNode *fn = in->PortFunctions[port];
//			usbbase->usb_IExec->DebugPrintF( "HUB : Removing Device : FN    %p : Port #%ld : Class %ld\n", fn, port, fn->fkt_Class );

			HUB__Port_Function_Rem( usbbase, in, port );
		}

		if ( stat.wPortStatus & PORTSTATF_Connection )
		{
			#ifdef DO_INFO
			usbbase->usb_IExec->Disable();
			usbbase->usb_IExec->DebugPrintF( "\nHUB : Adding Device\n" );
			usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
			usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
			usbbase->usb_IExec->Enable();
			#endif

			if ( ! HUB__Port_Function_Add( usbbase, in, port ))
			{
				USBERROR( "HUB__Port_Chg : HUB__Port_Function_Add() failed" );
			}
			else
			{
				struct RealFunctionNode *fn = in->PortFunctions[port];
//				usbbase->usb_IExec->DebugPrintF( "HUB : Added Device : FN    %p : Port #%ld : Class %ld\n", fn, port, fn->fkt_Class );
			}
		}
	}

	// -- Handle Port Enable

	if ( stat.wPortChange & PORTCHGF_Enable )
	{
		#ifdef DO_INFO
		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "\nHUB : Clearing Enable Change (todo)\n" );
		usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
		usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
		usbbase->usb_IExec->Enable();
		#endif

//		err = HUB__Port_Feature_Clr( usbbase, in, port, PORT_CLR_ENABLE );
//
//		if ( err != USB2Err_NoError )
//		{
//			USBDEBUG( "Error clearing Port Enable (%ld)", err );
//			goto bailout;
//		}
	}

	// -- Handle Port Suspend

	if ( stat.wPortChange & PORTCHGF_Suspend )
	{
		#ifdef DO_INFO
		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "\nHUB : Clearing Suspend Change (todo)\n" );
		usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
		usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
		usbbase->usb_IExec->Enable();
		#endif

//		err = HUB__Port_Feature_Clr( usbbase, in, port, PORT_CLR_SUSPEND );
//
//		if ( err != USB2Err_NoError )
//		{
//			USBDEBUG( "Error clearing Port Suspend (%ld)", err );
//			goto bailout;
//		}
	}

	// -- Handle Port Over Current

	if ( stat.wPortChange & PORTCHGF_Over_Current )
	{
		#ifdef DO_INFO
		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "\nHUB : Clearing OverCurrent Change (todo)\n" );
		usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
		usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
		usbbase->usb_IExec->Enable();
		#endif

//		err = HUB__Port_Feature_Clr( usbbase, in, port, PORT_CLR_OVER_CURRENT );
//
//		if ( err != USB2Err_NoError )
//		{
//			USBDEBUG( "Error clearing Port OverCurrent (%ld)", err );
//			goto bailout;
//		}
	}

	// -- Handle Port Reset

	if ( stat.wPortChange & PORTCHGF_Reset )
	{
		#ifdef DO_INFO
		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "\nHUB : Clearing Reset Change (todo)\n" );
		usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
		usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
		usbbase->usb_IExec->Enable();
		#endif

//		err = HUB__Port_Feature_Clr( usbbase, in, port, PORT_CLR_RESET );
//
//		if ( err != USB2Err_NoError )
//		{
//			USBDEBUG( "Error clearing Port Reset (%ld)", err );
//			goto bailout;
//		}
	}

	// --

bailout:

	return;
}
