
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
// if we started as a Task and then DOS is started,
// we promote a Task into a Process
//
// Well I start a process, change the Task Signals, MsgPorts and TaskAdrs to our new process
// I guess this is a dangerous process, that may not fail

S32 __myProcess_Reinit( struct USBBase *usbbase, struct intern *in );

SEC_CODE void Master_Promote_Entry( void )
{
struct MasterStartMsg *msg;
struct USB3_TaskNode *tn;
struct ExecIFace *IExec;
struct USBBase *usbbase;
struct intern *in;
struct Task *parent;
//struct Task *self;
U32 mask;

	IExec	= (PTR)(*(struct ExecBase **)4)->MainInterface;
	msg		= (PTR)(IExec->FindTask(NULL)->tc_UserData);

	usbbase	= msg->usbbase;
	parent	= msg->Parent;
	mask	= msg->Mask;
//	self	= TASK_FIND();

	msg->Result = FALSE;

	// --

	in = usbbase->usb_Master_Intern;

	tn = & usbbase->usb_Master_Task;
//	tn->tn_State = TASK_State_Starting;

	// --

	if ( __myProcess_Init( usbbase, in, TRUE ))
	{
		msg->Result = TRUE;
		TASK_SIGNAL( parent, mask );
		TASK_SETPRI( 25 );

		__myProcess_Main( usbbase, in );

		tn->tn_State = TASK_State_Stopping;

		// --

		parent	= usbbase->usb_Master_Task.tn_Parent;
		mask	= usbbase->usb_Master_Task.tn_Mask;

		tn->tn_TaskAdr = NULL;
	}

	__myProcess_Free( usbbase, in );

	usbbase->usb_IExec->Disable();

	tn->tn_State = TASK_State_Stopped;

	// --
	// Parent can be NULL
	TASK_SIGNAL( parent, mask );
}

// --
