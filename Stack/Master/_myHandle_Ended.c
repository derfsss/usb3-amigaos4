
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --
// This Subtask have been Shutdown, now handle stuff

static void myRemoveNode( 
	struct USB3_TaskMsg *tm, 
	struct USBBase *usbbase,
	struct intern *in )
{
	USBDEBUG( "--> Cmd :: __Cmd_Stop_Process       : 55 : Ended : " );

	// --
	// Timer

	if ( tm->tm_TimeReq_Active )
	{
		tm->tm_TimeReq_Active = FALSE;
		IO_STOP( & tm->tm_TimeRequest );
	}

	// --
	// Remove from List

	if ( tm->tm_Added )
	{
		tm->tm_Added = FALSE;
		NODE_REMNODE( & in->Shutdown_TM_List, tm );
	}

	TASKMSG_FREE( tm );
	tm = NULL;	// Just make sure we don't use it from now on

	// --
	// Inform Master (our self?) a task have ended
	TASK_SIGNAL( usbbase->usb_Master_Task.tn_TaskAdr, SIGBREAKF_CTRL_D );
}

// --

SEC_CODE static void myHandle_Task_Ended( struct USBBase *usbbase, struct intern *in )
{
struct USB3_TaskNode *tn;
struct USB3_TaskMsg *tm;
struct USB3_TaskMsg *next;
//struct ExecIFace *IExec;

//	IExec = usbbase->usb_IExec;

	USBDEBUG( "--> Cmd :: __Cmd_Stop_Process       : 66 : Ended : " );

	tm = in->Shutdown_TM_List.uh_Head;

	while( tm )
	{
		USBDEBUG( "--> Cmd :: __Cmd_Stop_Process       : 68 : Ended : TM    %p", tm );

		tn = tm->tm_Task;

		#if 0
		IExec->Disable();
		IExec->DebugPrintF( "myHandle_Task_Ended : TM    %p\n", tm );
		TASKNODE_PRINT(tn);
		IExec->DebugPrintF( "Next ............ : %08lx\n", (U32) Node_Next(tm) );
		IExec->DebugPrintF( "Prev ............ : %08lx\n", (U32) Node_Prev(tm) );
		IExec->DebugPrintF( "cm_Command ...... : %08lx\n", (U32) tm->tm_Cmd.cm_Command );
		IExec->DebugPrintF( "cm_usbbase ...... : %08lx\n", (U32) tm->tm_Cmd.cm_usbbase );
		IExec->DebugPrintF( "cm_Result ....... : %08lx\n", (U32) tm->tm_Cmd.cm_Result );
		IExec->DebugPrintF( "cm_Task ......... : %08lx\n", (U32) tm->tm_Cmd.cm_Task );
		IExec->DebugPrintF( "cm_Mask ......... : %08lx\n", (U32) tm->tm_Cmd.cm_Mask );
		IExec->DebugPrintF( "tm_Locks ........ : %08lx\n", (U32) tm->tm_Locks );
		IExec->DebugPrintF( "tm_Detach ....... : %08lx\n", (U32) tm->tm_Detach );
		IExec->DebugPrintF( "tm_StructID ..... : %08lx\n", (U32) tm->tm_StructID );
		IExec->DebugPrintF( "tm_Task ......... : %08lx (See above)\n", (U32) tn );
		IExec->DebugPrintF( "tm_Driver ....... : %08lx\n", (U32) tm->tm_Driver );
		IExec->DebugPrintF( "tm_ASync ........ : %08lx\n", (U32) tm->tm_ASync );
		IExec->DebugPrintF( "tm_Name ......... : %08lx\n", (U32) tm->tm_Name );
		IExec->DebugPrintF( "tm_Data ......... : %08lx\n", (U32) tm->tm_Data );
		IExec->DebugPrintF( "tm_Init ......... : %08lx\n", (U32) tm->tm_Func_Init );
		IExec->DebugPrintF( "tm_Entry ........ : %08lx\n", (U32) tm->tm_Func_Entry );
		IExec->DebugPrintF( "tm_Free ......... : %08lx\n", (U32) tm->tm_Func_Free );
		IExec->DebugPrintF( "tm_Prioity ...... : %ld\n", (S32) tm->tm_Prioity );
		IExec->DebugPrintF( "tm_StackSize .... : %08lx\n", (U32) tm->tm_StackSize );
		IExec->DebugPrintF( "tm_TimeReq_Active : %08lx\n", (U32) tm->tm_TimeReq_Active );
		IExec->DebugPrintF( "tm_Wait ......... : %08lx\n", (U32) tm->tm_Wait );
		IExec->DebugPrintF( "tm_Keep ......... : %08lx\n", (U32) tm->tm_Keep );
		IExec->DebugPrintF( "tm_Added ........ : %08lx\n", (U32) tm->tm_Added );
		IExec->Enable();
		#endif
	
		// --

		next = Node_Next(tm);

		tn = tm->tm_Task;

		if ( ! tn )
		{
//			USBDEBUG( "--> Cmd :: __Cmd_Stop_Process       : 61 : Ended : No TaskNode : TM    %p", tm );
			myRemoveNode( tm, usbbase, in );
		}
		else if ( ! tn->tn_TaskAdr )
		{
//			USBDEBUG( "--> Cmd :: __Cmd_Stop_Process       : 62 : Ended : Task Ended : TM    %p", tm );
			myRemoveNode( tm, usbbase, in );
		}

		tm = next;
	}

//	usbbase->usb_IExec->DebugPrintF( "myHandle_Task_Ended : 44 :\n" );
//	USBDEBUG( "--> Cmd :: __Cmd_Stop_Process       : 70 : Ended" );
}

// --
