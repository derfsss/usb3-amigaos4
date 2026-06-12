
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

SEC_CODE S32 HUB__HUB_Status_Get( struct USBBase *usbbase UNUSED, struct intern *in UNUSED, struct USB3_HUBStatus *stat UNUSED )
{
	// --

//	usbbase->usb_IExec->Disable();
//	usbbase->usb_IExec->DebugPrintF( "\nHUB : Get HUB Status\n" );
//	usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//	usbbase->usb_IExec->Enable();

	// --

	#if 0
struct USB3_IORequest *ioreq;
struct USB3_SetupData *sd;
S32 errcode;

//	  usbbase->usb_IExec->Disable();
//	  usbbase->usb_IExec->DebugPrintF( "\nGet HUB Status\n" );
//	  usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %ld\n", in->Function->fkt_Tier );
//	  usbbase->usb_IExec->Enable();

	ioreq	= in->Res_Control->IORequests[0];
	sd		= in->Res_Control->SetupData;

	sd->RequestType	= REQTYPE_Read | REQTYPE_Class | REQTYPE_Device;
	sd->RequestCode	= REQCODE_Get_Status;
	sd->Value		= LE_SWAP16( 0 );
	sd->Index		= LE_SWAP16( 0 );
	sd->Length		= LE_SWAP16( sizeof( struct USB3_HUBStatus ) );

	ioreq->io_Command		= CMD_READ;
	ioreq->io_Data			= stat;
	ioreq->io_Length		= sizeof( struct USB3_HUBStatus );
	ioreq->io_SetupData		= sd;
	ioreq->io_SetupLength	= sizeof( struct USB3_SetupData );

	IO_DO( ioreq );

	if (( ioreq->io_Error  == USB3Err_NoError )
	&&	( ioreq->io_Actual == sizeof( struct USB3_HUBStatus )))
	{
		stat->wHubStatus = LE_SWAP16( stat->wHubStatus );
		stat->wHubChange = LE_SWAP16( stat->wHubChange );

		USBDEBUG( "HUBStatus 0x%04lx", stat->wHubStatus );
		USBDEBUG( "HUBChange 0x%04lx", stat->wHubChange );

//		  usbbase->usb_IExec->Disable();
//		  usbbase->usb_IExec->DebugPrintF( "\nGet HUB Status Success\n" );
//		  usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//		  usbbase->usb_IExec->DebugPrintF( "HUB Status : 0x%04lx\n", stat->wHubStatus );
//		  usbbase->usb_IExec->DebugPrintF( "HUB Change : 0x%04lx\n", stat->wHubChange );
//		  usbbase->usb_IExec->Enable();

		errcode = USB3Err_NoError;
	}
	else
	{
		stat->wHubStatus = 0;
		stat->wHubChange = 0;

//		  usbbase->usb_IExec->Disable();
//		  usbbase->usb_IExec->DebugPrintF( "\nGet HUB Status Failed\n" );
//		  usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %ld\n", in->Function->fkt_Tier );
//		  usbbase->usb_IExec->Enable();

		errcode = -1;
	}

	return( errcode );
	#endif

	return( -1 );
}
