
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Init_1_Misc( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct intern *in )
{
struct Task *self;
	S32 retval;

	retval = FALSE;

	// --

	self = TASK_FIND();
	ASYNC_INIT( & hn->hn_HUB_ASync, self );

	// --
	// --- Max 128 USB Functions/Devices

	hn->hn_Function_Array = MEM_ALLOCVEC( 128 * sizeof( struct RealFunctionNode * ), TRUE );

	if ( ! hn->hn_Function_Array )
	{
		USBDEBUG( "__myInit (HCD)           : Error creating hn_Function_Array" );
		goto bailout;
	}

	// --- Max 32 Endpoint and Max 128 USB Devices

	hn->hn_EndPoint_Array = MEM_ALLOCVEC( 32 * 128 * sizeof( struct USB2_EndPointNode * ), TRUE );

	if ( ! hn->hn_EndPoint_Array )
	{
		USBDEBUG( "__myInit (HCD)           : Error creating hn_EndPoint_Array" );
		goto bailout;
	}

	// --- Alloc HUB Desc Memory

	hn->hn_HUB_Desc = MEM_ALLOCVEC( sizeof( struct USB2_HUB_Desc ), TRUE );

	if ( ! hn->hn_HUB_Desc )
	{
		USBDEBUG( "__myInit (HCD)           : Error allocating memory" );
		goto bailout;
	}

	// Root HUB Signal

	if ( ! TASK_ALLOCSIGNAL( & hn->hn_HUB_Signal ))
	{
		USBDEBUG( "__myInit (HCD)           : Error Obtaining PortChange Signal" );
		goto bailout;
	}

	// HCD Crashed .. we need a hcd chip reset signal

	if ( ! TASK_ALLOCSIGNAL( & hn->HCD_Restart_Chip ))
	{
		USBDEBUG( "__myInit (HCD)           : Error Obtaining Chip Crash Signal" );
		goto bailout;
	}
	
	// AbortIO MsgPort

	if ( ! MSGPORT_INIT( & hn->hn_Abort_MsgPort ))
	{
		USBDEBUG( "__myInit (HCD)           : Error: MsgPort init failed" );
		goto bailout;
	}

	// BeginIO MsgPort

	if ( ! MSGPORT_INIT( & hn->hn_Begin_MsgPort ))
	{
		USBDEBUG( "__myInit (HCD)           : Error: MsgPort init failed" );
		goto bailout;
	}

	// Timer MsgPort

	if ( ! MSGPORT_INIT( & hn->hn_Delay_MsgPort ))
	{
		USBDEBUG( "__myInit (HCD)           : Error: MsgPort init failed" );
		goto bailout;
	}

	MEM_COPY( usbbase->usb_TimeRequest, & hn->hn_Delay_IOReq, sizeof( struct TimeRequest ));

	hn->hn_Delay_IOReq.Request.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	hn->hn_Delay_IOReq.Request.io_Message.mn_ReplyPort = & hn->hn_Delay_MsgPort.ump_MsgPort;
	hn->hn_Delay_IOReq.Request.io_Message.mn_Length = sizeof( struct TimeRequest );
	hn->hn_Delay_IOReq.Request.io_Command = TR_ADDREQUEST;
	hn->hn_Delay_IOReq.Time.Microseconds = 0;
	hn->hn_Delay_IOReq.Time.Seconds = 2;

	// WatchDog MsgPort

	if ( ! MSGPORT_INIT( & hn->hn_WatchDog_MsgPort ))
	{
		USBDEBUG( "__myInit (HCD)           : Error: MsgPort init failed" );
		goto bailout;
	}

	// -- Timer Tick

	if ( ! MSGPORT_INIT( & in->Tick_MsgPort ))
	{
		goto bailout;
	}

	MEM_COPY( usbbase->usb_TimeRequest, & in->Tick_TimeRequest, sizeof( struct TimeRequest ));

	in->Tick_TimeRequest.Request.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	in->Tick_TimeRequest.Request.io_Message.mn_ReplyPort = & in->Tick_MsgPort.ump_MsgPort;
	in->Tick_TimeRequest.Request.io_Message.mn_Length = sizeof( struct TimeRequest );
	in->Tick_TimeRequest.Request.io_Command = TR_ADDREQUEST;
	in->Tick_TimeRequest.Time.Microseconds = 0;
	in->Tick_TimeRequest.Time.Seconds = 2;

	retval = TRUE;

bailout:

	return( retval );
}

// --
