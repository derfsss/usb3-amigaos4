
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

SEC_CODE static S32 __myTask_Def_Init( struct USBBase *usbbase, struct intern *in, S32 Promote )
{
struct USB3_TaskNode *tn;
struct ExecIFace *IExec;
S32 retval;

	retval = FALSE;
	USBINFO( "__myTask_Def_Init (Master)" );

	// 

	IExec = usbbase->usb_IExec;

	if ( Promote )
	{
		IExec->Disable();
	
		tn = & usbbase->usb_Master_Task;
		tn->tn_TaskAdr = TASK_FIND();
	
		TASK_REALLOCSIGNAL( & in->Task_Shutdown_Signal );
	
		MSGPORT_REINIT( & in->Task_Shutdown_MsgPort );
	
		MSGPORT_REINIT( & in->Cmd_MsgPort );
	
		MSGPORT_REINIT( & in->Tick_MsgPort );

		MSGPORT_REINIT( & in->Notify_ReplyMsgPort );

		IExec->Enable();
	}
	else
	{
		if ( ! TASK_ALLOCSIGNAL( & in->Task_Shutdown_Signal ))
		{
			USBERROR( "__myTask_Def_Init (Master) : 1 : Error" );
			goto bailout;
		}

		if ( ! MSGPORT_INIT( & in->Task_Shutdown_MsgPort ))
		{
			USBERROR( "__myTask_Def_Init (Master) : 2 : Error" );
			goto bailout;
		}

		// --

		if ( ! MSGPORT_INIT( & in->Cmd_MsgPort ))
		{
			USBERROR( "__myTask_Def_Init (Master) : 3 : Error" );
			goto bailout;
		}

		usbbase->usb_Master_MsgPort = & in->Cmd_MsgPort;

		// --

		if ( ! MSGPORT_INIT( & in->Notify_ReplyMsgPort ))
		{
			USBERROR( "__myTask_Def_Init (Master) : 3.2 : Error" );
			goto bailout;
		}

		usbbase->usb_Notify_ReplyMsgPort = & in->Notify_ReplyMsgPort;

		// -- Timer Tick

		if ( ! MSGPORT_INIT( & in->Tick_MsgPort ))
		{
			USBERROR( "__myTask_Def_Init (Master) : 4 : Error" );
			goto bailout;
		}
	
		MEM_COPY( usbbase->usb_TimeRequest, & in->Tick_TimeRequest, sizeof( struct TimeRequest ));

		in->Tick_TimeRequest.Request.io_Message.mn_ReplyPort = & in->Tick_MsgPort.ump_MsgPort;
		in->Tick_TimeRequest.Request.io_Command = TR_ADDREQUEST;
		in->Tick_TimeRequest.Time.Microseconds = 0;
		in->Tick_TimeRequest.Time.Seconds = 2;

		in->Tick_TimerAdded = TRUE;
		IO_SEND( & in->Tick_TimeRequest );
	}

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --
