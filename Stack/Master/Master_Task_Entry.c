
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
// If DOS is not around, we have to run in a Task

SEC_CODE void Master_Task_Entry( struct USBBase *usbbase, struct MasterStartMsg *msg )
{
struct USB2_TaskNode *tn;
struct intern *in;
struct Task *parent;
struct Task *self;
U32 mask;
S32 stat;
S32 rc;

	// --

	parent	= msg->Parent;
	mask	= msg->Mask;
	self	= TASK_FIND();

	// --

	rc = MAIN_RETURN_UNSET;
	in = usbbase->usb_Master_Intern;
	tn = & usbbase->usb_Master_Task;
	tn->tn_State = TASK_State_Starting;

	stat = __myTask_Init( usbbase, in );

	if ( stat )
	{
		tn->tn_TaskAdr = self;

		msg->Result = TRUE;
		TASK_SIGNAL( parent, mask );

		// --

		tn->tn_State = TASK_State_Running;

		rc = __myTask_Main( usbbase, in );

		if ( rc != MAIN_RETURN_PROMOTE )
		{
			tn->tn_State = TASK_State_Stopping;

			// --

			parent	= usbbase->usb_Master_Task.tn_Parent;
			mask	= usbbase->usb_Master_Task.tn_Mask;

			tn->tn_TaskAdr = NULL;
		}
	}

	if ( rc != MAIN_RETURN_PROMOTE )
	{
		__myTask_Free( usbbase, in );

		usbbase->usb_IExec->Disable();

		tn->tn_State = TASK_State_Stopped;

		// --
		// Parent can be NULL
		TASK_SIGNAL( parent, mask );
	}
}

// --
