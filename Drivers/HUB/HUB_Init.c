
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

SEC_CODE S32 HUB_Init( struct USBBase *usbbase, struct intern *in )
{
struct RealFunctionNode *fn;
//struct USB2_IORequest *ioreq;
//struct USB2_SetupData *sd;
struct Task *self;
S32 retval;

	retval = FALSE;

	USBDEBUG( "HUB : HUB_Init" );

	// --

	self = TASK_FIND();
	ASYNC_INIT( & in->ASync_Drivers, self );
	ASYNC_INIT( & in->ASync_Functions, self );

	// --

	in->Register = REGISTER_REGISTERTAGS(
		USB2Tag_Reg_DriverMessage, in->StartMsg,
		USB2Tag_Reg_TimeOut, 1000000*1,		// 1 sec
		USB2Tag_Reg_Title, "HUB",
		TAG_END
	);

	if ( ! in->Register )
	{
		USBERROR( "HUB_Init                  : Error Registering Device" );
		goto bailout;
	}

	in->Res_Control = in->Register->reg_Public.Res_Control;

//	ioreq	= in->Res_Control->IORequests[0];
//	sd		= in->Res_Control->SetupData;
	fn		= in->Function;

	USBDEBUG( "FN Setup Locks #%lu", fn->fkt_Locks );

	// --
	// Delay Timer MsgPort

	if ( ! MSGPORT_INIT( & in->Delay_MsgPort ))
	{
		USBERROR( "HUB_Init                  : Error: Delay MsgPort init failed" );
		goto bailout;
	}

	MEM_COPY( usbbase->usb_TimeRequest, & in->Delay_IOReq, sizeof( struct TimeRequest ));

	in->Delay_IOReq.Request.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	in->Delay_IOReq.Request.io_Message.mn_ReplyPort = & in->Delay_MsgPort.ump_MsgPort;
	in->Delay_IOReq.Request.io_Message.mn_Length = sizeof( struct TimeRequest );
	in->Delay_IOReq.Request.io_Command = TR_ADDREQUEST;
	in->Delay_IOReq.Time.Microseconds = 0;
	in->Delay_IOReq.Time.Seconds = 2;

	// --
	// Tick Timer MsgPort

	if ( ! MSGPORT_INIT( & in->Tick_MsgPort ))
	{
		USBERROR( "HUB_Init                  : Error: Tick MsgPort init failed" );
		goto bailout;
	}

	MEM_COPY( usbbase->usb_TimeRequest, & in->Tick_TimeRequest, sizeof( struct TimeRequest ));

	in->Tick_TimeRequest.Request.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	in->Tick_TimeRequest.Request.io_Message.mn_ReplyPort = & in->Tick_MsgPort.ump_MsgPort;
	in->Tick_TimeRequest.Request.io_Message.mn_Length = sizeof( struct TimeRequest );
	in->Tick_TimeRequest.Request.io_Command = TR_ADDREQUEST;
	in->Tick_TimeRequest.Time.Microseconds = 0;
	in->Tick_TimeRequest.Time.Seconds = 2;

	// --

	if ( ! HUB_GetDesc( usbbase, in ))
	{
		USBERROR( "HUB_Init                  : Error failed to get HUB Descriptor" );
	}

	// --

	in->PortFunctions = MEM_ALLOCVEC( sizeof( struct RealFunctionNode * ) * ( in->Ports + 1 ), TRUE );

	if ( ! in->PortFunctions )
	{
		USBERROR( "HUB_Init                  : Error allocating Memory" );
		goto bailout;
	}

	// ---

	in->Res_Interrupt = ENDPOINTRES_OBTAINTAGS( in->Register,
		USB2Tag_EPRes_EPDirection, EPADR_Dir_In,
		USB2Tag_EPRes_EPType, EPATT_Type_Interrupt,
		USB2Tag_EPRes_BufferSize, ( in->Ports + 7 ) / 8,
		TAG_END
	);

	if ( ! in->Res_Interrupt )
	{
		USBERROR( "HUB_Init                  : Error obtaining EndPoint Resource" );
		goto bailout;
	}

	// ---
	// -- Start Timer Tick
	in->Tick_TimerAdded = TRUE;
	IO_SEND( & in->Tick_TimeRequest );

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --
