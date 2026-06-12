
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

void Master_Task_Entry( struct USBBase *usbbase, struct USB3_TaskMsg *tm );

// --

SEC_CODE static S32 __myMaster_StartTask( struct USBBase *usbbase )
{
struct MasterStartMsg msg;
struct ExecIFace *IExec;
struct Task *task;
S32 retval;
U32 mask;

	retval = FALSE;

//	usbbase->usb_IExec->DebugPrintF( "__myMaster_StartTask 1 :\n" );

	if ( ! usbbase->usb_Master_Task.tn_TaskAdr )
	{
		msg.usbbase = usbbase;
		msg.Parent = TASK_FIND();
		msg.Result = FALSE;
		msg.Mask = SIGBREAKF_CTRL_E;

		IExec = usbbase->usb_IExec;

		task = IExec->CreateTaskTags(
			"Universal Serial Bus",
			25,
			Master_Task_Entry,
			1024 * 32,
			AT_Param1, usbbase,
			AT_Param2, & msg,
			TAG_END 
		);

		while( task )
		{
			mask = IExec->Wait( msg.Mask );

			if (( mask & msg.Mask ) == msg.Mask )
			{
				retval = msg.Result;
				break;
			}
		}
	}
	else
	{
		retval = TRUE;
	}

//	usbbase->usb_IExec->DebugPrintF( "__myMaster_StartTask 99 : Retval %ld\n", retval );

	return( retval );
}

// --
