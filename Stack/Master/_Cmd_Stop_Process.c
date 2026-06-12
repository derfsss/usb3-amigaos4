
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Cmd_Stop_Process( struct USBBase *usbbase, struct intern *in UNUSED, struct USB3_TaskMsg *tm )
{
struct USB3_TaskNode *tn;
S32 reply;

	tn = tm->tm_Task;

	USBINFO( "__Cmd_Stop_Process       :  1 : IN    %p : TM    %p : TN    %p :", in, tm, tn );

	if ((tn) && ( tn->tn_TaskAdr ))
	{
		TASKNODE_PRINT(tn);
		
		// -- Setup Exit Info and Signal Subtask
		tn->tn_Parent	= TASK_FIND();
		tn->tn_Mask		= in->Task_Shutdown_Signal.sig_Signal_Mask;
		TASK_SIGNAL( tn->tn_TaskAdr, SIGBREAKF_CTRL_C );

		// -- Setup Timer Request, and start Timer
		MEM_COPY( usbbase->usb_TimeRequest, & tm->tm_TimeRequest, sizeof( struct TimeRequest ));

		tm->tm_TimeReq_Active = TRUE;
		tm->tm_TimeRequest.Request.io_Message.mn_Node.ln_Name = (PTR) tm;
		tm->tm_TimeRequest.Request.io_Message.mn_ReplyPort = & in->Task_Shutdown_MsgPort.ump_MsgPort;
		tm->tm_TimeRequest.Request.io_Command = TR_ADDREQUEST;
		tm->tm_TimeRequest.Time.Seconds = 30;
		tm->tm_TimeRequest.Time.Microseconds = 0;
		IO_SEND( & tm->tm_TimeRequest );

		// -- 
		tm->tm_Keep = TRUE;	// Don't Free Msg
		tm->tm_Added = TRUE; // We have added msg to List, it now need a REMNODE
		NODE_ADDTAIL( & in->Shutdown_TM_List, tm );

		// --
		reply = TRUE;
	}
	else
	{
		// Since there are no Subtask, just reply right away
		tm->tm_Keep = FALSE;
		reply = TRUE;
	}

//	USBDEBUG( "__Cmd_Stop_Process       : 13 : TM    %p : TN    %p : Reply %ld :", tm, tn, reply );

	return( reply );
}

// --

SEC_CODE static void myHandle_Task_Timeout( struct USBBase *usbbase, struct intern *in )
{
struct USB3_TaskMsg *tm;
struct Message *msg;

	while( TRUE )
	{
		msg = MSGPORT_GETMSG( & in->Task_Shutdown_MsgPort );
		
		if ( ! msg )
		{
			break;
		}

		tm = (PTR) msg->mn_Node.ln_Name;

		// -- if we get here, a subtask have not shutdown
		// for now.. I just restart the timer
//		usbbase->usb_IExec->DebugPrintF( "Task Shutdown Timeout .. uhh what now\n" );
		USBINFO( "myHandle_Task_Timeout : Task shutdown Timerout : TM    %p : TN    %p : DN    %p :", tm, tm->tm_Task, tm->tm_Driver );
//		USBDEBUG( "--> Cmd :: __Cmd_Stop_Process       : 55 : Timeout : TM    %p", tm );

		// -- restart timer
		tm->tm_TimeRequest.Time.Seconds = 5;
		tm->tm_TimeRequest.Time.Microseconds = 0;
		IO_SEND( & tm->tm_TimeRequest );
	}
}

// --
