
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

SEC_CODE S32 HUB__Port_Status_Get( struct USBBase *usbbase, struct intern *in, U32 port, struct USB3_PortStatus *stat )
{
struct USB3_IORequest *ioreq;
struct USB3_SetupData *sd;
S32 errcode;

	// --

//	usbbase->usb_IExec->Disable();
//	usbbase->usb_IExec->DebugPrintF( "\nHUB : Get Port Status\n" );
//	usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//	usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
//	usbbase->usb_IExec->Enable();

	// --

	#ifdef DO_STABLE

	if (( port == 0 ) || ( port > in->Ports ))
	{
		errcode = -1;
		USBDEBUG( "HUB__Port_Status_Get : Illegal Port Number (%lu)", port );
		goto bailout;
	}

	#endif

	// --

	ioreq	= in->Res_Control->IORequests[0];
	sd		= in->Res_Control->SetupData;

	sd->RequestType		= REQTYPE_Read | REQTYPE_Class | REQTYPE_Other;
	sd->RequestCode		= REQCODE_Get_Status;
	sd->Value			= LE_SWAP16( 0 );
	sd->Index			= LE_SWAP16( port );
	sd->Length			= LE_SWAP16( sizeof( struct USB3_PortStatus ) );

	ioreq->io_Command		= CMD_READ;
	ioreq->io_Data			= stat;
	ioreq->io_Length		= sizeof( struct USB3_PortStatus );
	ioreq->io_SetupData		= sd;
	ioreq->io_SetupLength	= sizeof( struct USB3_SetupData );

	USBDEBUG( "Get Port Status #%lu", port );

	IO_DO( ioreq );

	if (( ioreq->io_Error  == USB3Err_NoError )
	&&	( ioreq->io_Actual == sizeof( struct USB3_PortStatus )))
	{
		stat->wPortStatus = LE_SWAP16( stat->wPortStatus );
		stat->wPortChange = LE_SWAP16( stat->wPortChange );

//		usbbase->usb_IExec->Disable();
//		usbbase->usb_IExec->DebugPrintF( "\nHUB : Get Port Status Success\n" );
//		usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//		usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
//		usbbase->usb_IExec->DebugPrintF( "Port Status : 0x%04lx\n", stat->wPortStatus );
//		usbbase->usb_IExec->DebugPrintF( "Port Change : 0x%04lx\n", stat->wPortChange );
//		usbbase->usb_IExec->Enable();

		errcode = USB3Err_NoError;


		#ifdef DO_DEBUG
		{
			U32 val;

			val = stat->wPortStatus;

			USBDEBUG( "Port   #%ld", port );
			USBDEBUG( "Status $%04lx : %s%s%s%s%s%s%s%s%s%s",
				( val ),
				( val & PORTSTATF_Connection )		? "Current Connect Status, ": "",
				( val & PORTSTATF_Enable )			? "Port Enable, "			: "",
				( val & PORTSTATF_Suspend )			? "Port Suspend, "			: "",
				( val & PORTSTATF_Over_Current )	? "Over Current active, "	: "",
				( val & PORTSTATF_Reset )			? "Port Reset, "			: "",
				( val & PORTSTATF_Power )			? "Port Power, "			: "",
				( val & PORTSTATF_Low_Speed )		? "Low Speed, "				: "",
				( val & PORTSTATF_High_Speed )		? "High Speed, "			: "",
				( val & PORTSTATF_Test )			? "Test, "					: "",
				( val & PORTSTATF_Indicator )		? "Indicator"				: ""
			);

			val = stat->wPortChange;

			USBDEBUG( "Change $%04lx : %s%s%s%s%s",
				( val ),
				( val & PORTCHGF_Connection )		? "Connect Change, "		: "",
				( val & PORTCHGF_Enable )			? "Port Enable Change, "	: "",
				( val & PORTCHGF_Suspend )			? "Port Suspend Change, "	: "",
				( val & PORTCHGF_Over_Current )		? "Over Current Change, "	: "",
				( val & PORTCHGF_Reset )			? "Port Reset Change"		: ""
			);
		}
		#endif
	}
	else
	{
		stat->wPortStatus = 0;
		stat->wPortChange = 0;

//		usbbase->usb_IExec->Disable();
//		usbbase->usb_IExec->DebugPrintF( "\nHUB : Get Port Status Failure\n" );
//		usbbase->usb_IExec->DebugPrintF( "HUB Tier    : %lu\n", in->Function->fkt_Tier );
//		usbbase->usb_IExec->DebugPrintF( "Port        : %lu\n", port );
//		usbbase->usb_IExec->Enable();
	
		errcode = -1;
	}

	errcode = ioreq->io_Error;

	#ifdef DO_STABLE
	bailout:
	#endif

	return( errcode );
}

// --
