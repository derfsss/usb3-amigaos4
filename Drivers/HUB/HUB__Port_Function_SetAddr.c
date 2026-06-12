
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

SEC_CODE U32 HUB__Port_Function_SetAddr( struct USBBase *usbbase, struct intern *in, struct RealFunctionNode *fn )
{
struct USB3_IORequest *ioreq;
struct USB3_SetupData *sd;
U32 retval;
U32 adr;

	retval = FALSE;

	// -- Get Address

	adr = fn->fkt_Address;

	#ifdef DO_DEBUG

	if (( adr == 0 ) || ( adr > 127 ))
	{
		USBPANIC( "HUB__Port_Function_SetAddr : We can't use Address #%lu", adr );
	}

	#endif

	// -- Can't reuse IOReq, get a new

//	usbbase->usb_IExec->DebugPrintF( "HUB__Port_Function_SetAddr : New IOReq : Adr %ld\n", adr );

	ioreq = (PTR) IOREQUEST_ALLOCTAGS(
		USB3Tag_IOReq_EndPoint, fn->fkt_ControlEndPoint,
		TAG_END
	);

	if ( ! ioreq )
	{
		USBDEBUG( "HUB__Port_Function_SetAddr : Error creating IORequest ( FN: %p )", fn );
		goto bailout;
	}

	// -- Send IORequest

	sd = in->Res_Control->SetupData;
	sd->RequestType		= REQTYPE_Write | REQTYPE_Standard | REQTYPE_Device;
	sd->RequestCode		= REQCODE_Set_Address;
	sd->Value			= LE_SWAP16( adr );
	sd->Index			= LE_SWAP16( 0 );
	sd->Length			= LE_SWAP16( 0 );

	ioreq->io_Command		= CMD_WRITE;
	ioreq->io_SetupLength	= sizeof( struct USB3_SetupData );
	ioreq->io_SetupData		= sd;
	ioreq->io_Data			= NULL;
	ioreq->io_Length		= 0;

//	usbbase->usb_IExec->DebugPrintF( "HUB__Port_Function_SetAddr : Adr %ld : Do Set Address\n", (S32) adr );
	fn->fkt_Address = 0;
	IO_DO( ioreq );
	fn->fkt_Address = adr;
//	usbbase->usb_IExec->DebugPrintF( "HUB__Port_Function_SetAddr : Adr %ld : Set Address Returned (io_Err: %ld)\n", (S32) adr, (S32) ioreq->io_Error );

	if ( ioreq->io_Error != USB3Err_NoError )
	{
		USBDEBUG( "SetAddr - io_Error %ld ( FN: %p )", ioreq->io_Error, fn );
		goto bailout;
	}

	// Allow device time to set new address
	HUB_DELAY_MS( USB_SET_ADDRESS_SETTLE );

	retval = TRUE;

bailout:

	if ( ioreq )
	{
//		usbbase->usb_IExec->DebugPrintF( "HUB__Port_Function_SetAddr : Free IOReq\n" );

		IOREQUEST_FREE( (PTR) ioreq );
	}

	return( retval );
}

// --
