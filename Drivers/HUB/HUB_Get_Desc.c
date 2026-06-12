
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

SEC_CODE U32 HUB_GetDesc( struct USBBase *usbbase, struct intern *in )
{
struct RealFunctionNode *fn;
struct USB2_IORequest *ioreq;
struct USB2_SetupData *sd;
struct USB2_HUB_Desc *desc;
U32 dsctype;
U32 retval;
U32 max;
U32 cnt;

	retval	= FALSE;
	fn		= in->Function;
	desc	= (PTR) fn->fkt_HUBDescriptor;
	sd		= in->Res_Control->SetupData;
	ioreq	= in->Res_Control->IORequests[0];

	/*
	** Super-Speed hubs need to know their depth to be able to
	** parse the bits of the route-string that correspond to
	** their downstream port number.
	**
	** This does not apply to root hubs.
	*/

//	if (( fkt->fkt_Tier != 0 ) && ( fkt->fkt_Speed >= USBSPEED_Super ))
//	{
//		USBDEBUG( "Setting HUB Depth" );
//
//		sd->RequestType			= REQTYPE_Write | REQTYPE_Class | REQTYPE_Device;
//		sd->RequestCode			= REQCODE_Set_Depth;
//		sd->Value				= LE_SWAP16( fn->fkt_Tier - 1 );
//		sd->Index				= LE_SWAP16( 0 );
//		sd->Length				= LE_SWAP16( 0 );
//
//		ioreq->io_Command		= CMD_WRITE;
//		ioreq->io_Data			= 0;
//		ioreq->io_Length		= 0;
//		ioreq->io_SetupData		= sd;
//		ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );
//
//		IO_DO( ioreq );
//
//		if ( ioreq->io_Error )
//		{
//			USBDEBUG( "Error setting HUB Depth" );
//			goto bailout;
//		}
//	}

//	if ( fkt->fkt_Speed < USBSPEED_Super )
	{
		USBDEBUG( "HUB_GetDesc : Retrieving HUB Descriptor" );

		dsctype = DSCTYPE_HUB;
	}
//	else
//	{
//		USBDEBUG( "Retrieving HUB SS Descriptor" );
//
//		sd->Value = LE_SWAP16( DSCTYPE_SS_HUB << 8 );
//	}

	// -- Get Descriptor

	cnt = 0;
	max = 3;

	while( TRUE )
	{
		// Small, delay for retries
		HUB_DELAY_MS( 25 );

		// Check max retries
		if ( max < cnt++ )
		{
			USBDEBUG( "HUB_GetDesc : Error retriveing HUB Descriptor" );
			goto bailout;
		}

		// First try and get all in one go .. 255 max size
		sd->RequestType			= REQTYPE_Read | REQTYPE_Class | REQTYPE_Device;
		sd->RequestCode			= REQCODE_Get_Descriptor;
		sd->Value				= LE_SWAP16( dsctype << 8 );
		sd->Index				= LE_SWAP16( 0 );
		sd->Length				= LE_SWAP16( 255 );

		ioreq->io_Command		= CMD_READ;
		ioreq->io_Data			= fn->fkt_HUBDescriptor;
		ioreq->io_Length		= 255;
		ioreq->io_SetupData		= sd;
		ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

		IO_DO(ioreq);

		if ( ioreq->io_Error == 0 )
		{
			// yay success
			break;
		}

		// Get first 2 bytes .. that is the desc length and type
		sd->RequestType			= REQTYPE_Read | REQTYPE_Class | REQTYPE_Device;
		sd->RequestCode			= REQCODE_Get_Descriptor;
		sd->Value				= LE_SWAP16( dsctype << 8 );
		sd->Index				= LE_SWAP16( 0 );
		sd->Length				= LE_SWAP16( 2 );

		ioreq->io_Command		= CMD_READ;
		ioreq->io_Data			= desc;
		ioreq->io_Length		= 2;
		ioreq->io_SetupData		= sd;
		ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

		IO_DO(ioreq);

		if ( ioreq->io_Error != 0 )
		{
			// fail .. try again
			continue;
		}

		// Now we know the length, get all
		sd->RequestType			= REQTYPE_Read | REQTYPE_Class | REQTYPE_Device;
		sd->RequestCode			= REQCODE_Get_Descriptor;
		sd->Value				= LE_SWAP16( dsctype << 8 );
		sd->Index				= LE_SWAP16( 0 );
		sd->Length				= LE_SWAP16( desc->Length );

		ioreq->io_Command		= CMD_READ;
		ioreq->io_Data			= desc;
		ioreq->io_Length		= desc->Length;
		ioreq->io_SetupData		= sd;
		ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

		IO_DO(ioreq);

		if ( ioreq->io_Error == 0 )
		{
			// yay success
			break;
		}
	}

	if ( desc->DescriptorType != dsctype )
	{
		USBDEBUG( "HUB_GetDesc : Invalid HUB Descriptor type" );
		goto bailout;
	}

	// --

	in->Ports = fn->fkt_HUBDescriptor->NbrPorts;

	USBDEBUG( "HUB_GetDesc : HUB Ports %ld", in->Ports );

	if ( ! in->Ports )
	{
		USBDEBUG( "HUB_GetDesc : Error Zero Ports" );
		goto bailout;
	}

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --
