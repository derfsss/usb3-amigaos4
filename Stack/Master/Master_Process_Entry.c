
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
// If DOS is around, we can start Process right away

SEC_CODE void Master_Process_Entry( void )
{
struct MasterStartMsg *msg;
struct USB3_TaskNode *tn;
struct ExecIFace *IExec;
struct USBBase *usbbase;
struct intern *in;
struct Task *parent;
struct Task *self;
U32 mask;
S32 stat;

	IExec	= (PTR)(*(struct ExecBase **)4)->MainInterface;
	msg		= (PTR)(IExec->FindTask(NULL)->tc_UserData);

	usbbase	= msg->usbbase;
	parent	= msg->Parent;
	mask	= msg->Mask;
	self	= TASK_FIND();

	msg->Result = FALSE;

	// --

	in = usbbase->usb_Master_Intern;
	tn = & usbbase->usb_Master_Task;
	tn->tn_State = TASK_State_Starting;

	// --

	stat = __myProcess_Init( usbbase, in, FALSE );

	if ( stat )
	{
		tn->tn_TaskAdr = self;

		msg->Result = TRUE;
		TASK_SIGNAL( parent, mask );

		// --

		tn->tn_State = TASK_State_Running;

		__myProcess_Main( usbbase, in );

		tn->tn_State = TASK_State_Stopping;

		// --

		parent	= usbbase->usb_Master_Task.tn_Parent;
		mask	= usbbase->usb_Master_Task.tn_Mask;

		tn->tn_TaskAdr = NULL;
	}

	__myProcess_Free( usbbase, in );

	IExec->Disable();

	tn->tn_State = TASK_State_Stopped;

	// --
	// Parent can be NULL
	TASK_SIGNAL( parent, mask );
}

// --
