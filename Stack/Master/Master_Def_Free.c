
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

SEC_CODE static void __myTask_Def_Free( struct USBBase *usbbase, struct intern *in )
{
	USBINFO( "__myTask_Def_Free (Master)" );

	// --

	usbbase->usb_Master_MsgPort = NULL;
	MSGPORT_FREE( & in->Cmd_MsgPort );

	// --

	while( TRUE )
	{
		PTR msg = MSGPORT_GETMSG( & in->Notify_ReplyMsgPort );

		if ( ! msg )
		{
			break;
		}

		MEMORY_FREE( MEMID_NotifyMessage, msg, 0 );
	}

	MSGPORT_FREE( & in->Notify_ReplyMsgPort );

	// --

	MSGPORT_FREE( & in->Task_Shutdown_MsgPort );
	TASK_FREESIGNAL( & in->Task_Shutdown_Signal );

	// -- Timer Tick

	if ( in->Tick_TimerAdded )
	{
		in->Tick_TimerAdded = FALSE;
		IO_STOP( & in->Tick_TimeRequest );
	}

	MSGPORT_FREE( & in->Tick_MsgPort );

	// --
}

// --
