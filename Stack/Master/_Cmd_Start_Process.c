
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void CmdStart_Entry( struct USB3_TaskMsg *tm )
{
S32		( * init )	( struct USBBase *usbbase, struct USB3_DriverNode *dn, PTR in );
void	( * main )	( struct USBBase *usbbase, struct USB3_DriverNode *dn, PTR in );
void	( * free )	( struct USBBase *usbbase, struct USB3_DriverNode *dn, PTR in );
struct USB3_TaskNode *tn;
struct USB3_HCDNode *user_hn;
struct USB3_ASync *as;
struct ExecIFace *IExec;
struct USBBase *usbbase;
struct Task *self;
PTR user_data;
STR task_name;
STR org_name;
STR new_name;
U32 stat;
S32 wait;
PTR in;

	// Get Pointers, before msg is freed
	usbbase 	= tm->tm_Cmd.cm_usbbase;
	USBDEBUG( "CmdStart_Entry           :  1 : Enter" );

	// Reset return Status
	tm->tm_Cmd.cm_Result = TASK_Return_Stack_Error;
	main		= tm->tm_Func_Entry;
	init		= tm->tm_Func_Init;
	free		= tm->tm_Func_Free;
	wait		= tm->tm_Wait_For_Term;
	as			= tm->tm_ASync;
	tn			= tm->tm_Task;
	task_name	= tm->tm_Name;
	user_data	= tm->tm_UserData;
	user_hn		= NULL;
	in			= NULL;

	// --

	ASYNC_ADD( as );

	// --
	// Set task name

	self = TASK_FIND();
	org_name = self->tc_Node.ln_Name;
	new_name = NULL;

	if ( task_name )
	{
		new_name = MEM_PRINTF( "USB ' %s ' driver", task_name );
	}

	if ( new_name )
	{
		self->tc_Node.ln_Name = new_name;
	}

	// --

	while( user_data )
	{
		if ( tn->tn_Type != TASK_Type_HCD )
		{
			break;
		}

		#ifdef DO_DEBUG

		if ( tn->tn_Owner )
		{
			USBPANIC( "CmdStart_Entry           :: Already set tn_Owner  %p", tn->tn_Owner );
		}

		#endif

		struct USB3_HCDNode *hn = user_data;

		#ifdef DO_DEBUG

		if ( hn->hn_Task )
		{
			USBPANIC( "CmdStart_Entry           :: Already set hn_Task  %p", hn->hn_Task );
		}

		#endif

		if ( HCD_VALID( hn ) != VSTAT_Okay )
		{
			break;
		}

		if ( TASK_LOCK(tn) == LSTAT_Okay )
		{
			tn->tn_Owner = hn;
			hn->hn_Task = tn;

			// Lock installed
			user_hn = hn;
		}	
		break;
	}

	// --

	tn->tn_TaskAdr	= NULL;
	tn->tn_State	= TASK_State_Starting;
	tn->tn_Parent	= tm->tm_Cmd.cm_Task;
	tn->tn_Mask		= tm->tm_Cmd.cm_Mask;

	// Alloc Intern Struct
	if ( tm->tm_InternSize )
	{
		in = MEM_ALLOCVEC( tm->tm_InternSize, TRUE );

		if ( ! in )
		{
			USBDEBUG( "CmdStart_Entry           :: 3 : Error Allocating Memory (%ld)", tm->tm_InternSize );
			goto bailout;
		}
	}

	// --

	#ifdef DO_DEBUG

	// Get Current Signals
	U32 old_signals = TASK_SETSIGNAL(0,0);

	#endif

	// -- Call Init func if Any
	if ( init )
	{
		stat = init( usbbase, user_data, in );
	}
	else
	{
		stat = TRUE;
	}

	if ( ! myIS_TASKRETURN_ERR(stat) )
	{
		tm->tm_Cmd.cm_Result = TASK_Return_Stack_Okay;
		tm = NULL; // Can't use tm any more
		// --
		tn->tn_State = TASK_State_Running;
		tn->tn_TaskAdr = TASK_FIND();

		if ( ! wait )
		{
			TASK_SIGNAL( tn->tn_Parent, tn->tn_Mask );
			tn->tn_Parent = NULL;
			tn->tn_Mask = 0;
		}

		if ( main )
		{
			main( usbbase, user_data, in );
		}
		else
		{
			// if root hub, we really need to send CTRL+E
			// but only if main is not set
		}
	}
	else
	{
		tm->tm_Cmd.cm_Result = myGET_TASKRETURN_CODE(stat);
		tm = NULL; // Can't use tm any more
	}

	// -- Start Shutdown
	tn->tn_State = TASK_State_Stopping;
	tn->tn_TaskAdr = NULL;

	// -- Call Free func if Any
	if ( free )
	{
		free( usbbase, user_data, in );
	}

	// --

	#ifdef DO_DEBUG

	// Get Current Signals
	U32 new_signals = TASK_SETSIGNAL(0,0);

	if ( old_signals ^ new_signals )
	{
		usbbase->usb_IExec->DebugPrintF( "Unfreed Singal : Old $%08lx, New: $%08lx : Name '%s'\n", 
			old_signals,
			new_signals,
			self->tc_Node.ln_Name
		);
	}

	#endif

	// --

bailout:

	if ( in )
	{
		MEM_FREEVEC( in );
	}

	// --
	// Restore task name
	// nb: MEM_FREE breaks Disable

	self->tc_Node.ln_Name = org_name;

	if ( new_name )
	{
		MEM_FREEVEC( new_name );
	}

	// --
	// Exit - time
	IExec = usbbase->usb_IExec;
	IExec->Disable();

	// --

	ASYNC_SUB( as );

	// --
	// Remove lock

	if ( user_hn )
	{
		user_hn->hn_Task = NULL;
		tn->tn_Owner = NULL;
		TASK_UNLOCK(tn);
	}

	// --
	// Task is Done
	tn->tn_State = TASK_State_Stopped;

	// --
	// Parent can be NULL, we can handle that
	TASK_SIGNAL( tn->tn_Parent, tn->tn_Mask );
	TASK_SIGNAL( usbbase->usb_Master_Task.tn_TaskAdr, SIGBREAKF_CTRL_D );

	USBDEBUG( "CmdStart_Entry           :  2 : Leave" );
}

// --

SEC_CODE static void CmdStart_Process( void )
{
struct USB3_TaskMsg *tm;
struct ExecIFace *IExec;

	IExec	= (PTR)(*(struct ExecBase **)4)->MainInterface;
	tm		= (PTR)(IExec->FindTask(NULL)->tc_UserData);

	CmdStart_Entry(tm);
}

// --

SEC_CODE static S32 __Cmd_Start_Process( struct USBBase *usbbase, struct intern *in UNUSED, struct USB3_TaskMsg *tm )
{
struct USB3_TaskNode *tn;
struct ExecIFace *IExec;
struct DOSIFace *IDOS;
struct Task *task;
S32 reply;

	USBDEBUG( "__Cmd_Start_Process : 11 : TM    %p", tm );

	IExec = usbbase->usb_IExec;
	IDOS  = usbbase->usb_IDOS;
	reply = TRUE;
	task  = NULL;

	tn = TASK_ALLOC( tm->tm_ASync );

	if ( ! tn )
	{
		USBERROR( "__Cmd_Start_Process : Error allocating memory" );
		goto bailout;
	}

	// --
	tn->tn_Type	= tm->tm_Type;

	// --
	// Small input validator
	tm->tm_StackSize	= MAX( tm->tm_StackSize, 1024 * 32 );
	tm->tm_Prioity		= MAX( tm->tm_Prioity, -100 );
	tm->tm_Prioity		= MIN( tm->tm_Prioity, +100 );
	// --
	if ( TASK_LOCK(tn) != LSTAT_Okay )
	{
		USBDEBUG( "__Cmd_Start_Process : Error locking TN %p", tn );
		goto bailout;
	}
	tm->tm_Task	= tn;
	// --

	if ( IDOS )
	{
		USBDEBUG( "__Cmd_Start_Process : 12 : Process" );

		task = (PTR) IDOS->CreateNewProcTags( 
			NP_Name,		"USB3 - Process Initialing",
			NP_Child,		TRUE,
			NP_Priority,	tm->tm_Prioity,
			NP_StackSize,	tm->tm_StackSize,
			NP_Entry,		CmdStart_Process,
			NP_UserData,	tm,
			TAG_END 
		);
	}
	else
	{
		USBDEBUG( "__Cmd_Start_Process : 13 : Task" );

		task = IExec->CreateTaskTags(
			"USB3 - Task Initialing",
			tm->tm_Prioity,
			CmdStart_Entry,
			tm->tm_StackSize,
			AT_Param1, tm,
			TAG_END 
		);
	}

	if ( task )
	{
//		USBDEBUG( "__Cmd_Start_Process : 14 : TN    %p : Okay", tn );
		SEMAPHORE_OBTAIN(	& usbbase->usb_Task_Semaphore );
		NODE_ADDTAIL(		& usbbase->usb_Task_Header, tn );
		SEMAPHORE_RELEASE(	& usbbase->usb_Task_Semaphore );
		reply = FALSE;
	}
	else
	{
//		USBDEBUG( "__Cmd_Start_Process : 15 : TN    %p : Failed", tn );
		// Failed to start Process, we need to reply
		TASK_FREE(tn);
		reply = TRUE;
		tn = NULL;
	}

bailout:

	if ( tm->tm_Get_TaskNode )
	{
		*tm->tm_Get_TaskNode = tn;
	}

//	USBDEBUG( "__Cmd_Start_Process : 16 : TN    %p : Reply %ld :", task, reply );

	return( reply );
}

// --
