
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

SEC_CODE S32 HUB__Port_Feature_Set( struct USBBase *usbbase, struct intern *in, U32 port, U32 sel )
{
//struct RealFunctionNode *fkt;
struct USB3_IORequest *ioreq;
struct USB3_SetupData *sd;
S32 errcode;

	// --

//	usbbase->usb_IExec->Disable();
//	usbbase->usb_IExec->DebugPrintF( "\nHUB : Set Port Feature\n" );
//	usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//	usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
//	usbbase->usb_IExec->DebugPrintF( "Sel         : %lu\n", sel );
//	usbbase->usb_IExec->Enable();

	// --

	#ifdef DO_STABLE

	if (( port == 0 ) || ( port > in->Ports ))
	{
		errcode = -1;
		USBDEBUG( "HUB__Port_Feature_Set : Illegal Port Number (%lu)", port );
		goto bailout;
	}

	#endif

	// --

	ioreq	= in->Res_Control->IORequests[0];
	sd		= in->Res_Control->SetupData;

	sd->RequestType	= REQTYPE_Write | REQTYPE_Class | REQTYPE_Other;
	sd->RequestCode	= REQCODE_Set_Feature;
	sd->Value		= LE_SWAP16( sel );
	sd->Index		= LE_SWAP16( port );
	sd->Length		= LE_SWAP16( 0 );

	ioreq->io_Command		= CMD_WRITE;
	ioreq->io_Data			= NULL;
	ioreq->io_Length		= 0;
	ioreq->io_SetupData		= sd;
	ioreq->io_SetupLength	= sizeof( struct USB3_SetupData );

	#ifdef DO_DEBUG
	{
		STR txt;

		switch( sel )
		{
			case PORT_CONNECTION:		txt = "Connection"; break;
			case PORT_ENABLE:			txt = "Enable";	break;
			case PORT_SUSPEND:			txt = "Suspend"; break;
			case PORT_OVER_CURRENT:		txt = "Over Current"; break;
			case PORT_RESET:			txt = "Reset"; break;
			case PORT_POWER:			txt = "Power"; break;
			case PORT_LOW_SPEED:		txt = "Low Speed"; break;
			case PORT_HIGH_SPEED:		txt = "High Speed"; break;
			case PORT_TEST:				txt = "Test"; break;
			case PORT_INDICATOR:		txt = "Indicator"; break;
			case PORT_CLR_CONNECTION:	txt = "Connection Change"; break;
			case PORT_CLR_ENABLE:		txt = "Enable Change"; break;
			case PORT_CLR_SUSPEND:		txt = "Suspend Change"; break;
			case PORT_CLR_OVER_CURRENT:	txt = "Over Current Change"; break;
			case PORT_CLR_RESET:		txt = "Reset change"; break;
			default: 					txt = "Unknown"; break;
		}

//usbbase->usb_IExec->DebugPrintF( "Port %ld, Set Port %s Feature\n", port, txt );
		USBDEBUG( "Set Port %s Feature", txt );
	}
	#endif

	IO_DO( ioreq );

	if (( ioreq->io_Error  == USB3Err_NoError )
	&&	( ioreq->io_Actual == 0 ))
	{
//		  usbbase->usb_IExec->Disable();
//		  usbbase->usb_IExec->DebugPrintF( "\nSet Port Feature Success\n" );
//		  usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//		  usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
//		  usbbase->usb_IExec->DebugPrintF( "Sel         : %ld\n", sel );
//		  usbbase->usb_IExec->Enable();

		errcode = USB3Err_NoError;

	}
	else
	{
//		  usbbase->usb_IExec->Disable();
//		  usbbase->usb_IExec->DebugPrintF( "\nSet Port Feature Failure\n" );
//		  usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//		  usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
//		  usbbase->usb_IExec->DebugPrintF( "Sel         : %ld\n", sel );
//		  usbbase->usb_IExec->Enable();

		errcode = -1;
	}

	#ifdef DO_STABLE
	bailout:
	#endif

	return( errcode );
}
