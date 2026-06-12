
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if 0
struct USB2_TaskNode
{
	struct USB2_Node				tn_Node;
	// -- 
	U32								tn_StructID;
	S32								tn_Locks;
	U16								tn_Detach;
	U16								tn_FreeMe;
	//	struct USB2_TaskNode *			tn_TaskAdr; 
	struct USB2_ASync *				tn_ASync;
	// --
	struct Task *					tn_TaskAdr;
	struct Task *					tn_Parent;
	U32								tn_Mask;
	enum TaskState					tn_State;
	enum TaskType					tn_Type;
	PTR								tn_Owner;			// Points to a struct like 'struct USB2_HCDNode' see Type
};
#endif

#ifdef DO_DEBUG
SEC_CODE void __TaskNode_Print( struct USBBase *usbbase, struct USB2_TaskNode *tn )
{
struct ExecIFace *IExec;

	IExec = usbbase->usb_IExec;

	IExec->Disable();
	IExec->DebugPrintF( "-- TaskNode : %p :\n", tn );

	if ( ! tn )
	{
		IExec->DebugPrintF( " tn_Locks ....... : --\n" );
		IExec->DebugPrintF( " tn_ASync ....... : --\n" );
		IExec->DebugPrintF( " tn_Detach ...... : --\n" );
		IExec->DebugPrintF( " tn_FreeMe ...... : --\n" );
		IExec->DebugPrintF( " tn_TaskAdr ..... : --\n" );
		IExec->DebugPrintF( " tn_Parent ...... : --\n" );
		IExec->DebugPrintF( " tn_Mask ........ : --\n" );
		IExec->DebugPrintF( " tn_State ....... : --\n" );
		IExec->DebugPrintF( " tn_Type ........ : --\n" );
	}
	else
	{
		IExec->DebugPrintF( " tn_Locks ....... : %ld\n", tn->tn_Locks );
		IExec->DebugPrintF( " tn_ASync ....... : %p\n", tn->tn_ASync );
		IExec->DebugPrintF( " tn_Detach ...... : %ld\n", (S32) tn->tn_Detach );
		IExec->DebugPrintF( " tn_FreeMe ...... : %ld\n", (S32) tn->tn_FreeMe );
		IExec->DebugPrintF( " tn_TaskAdr ..... : %p\n", tn->tn_TaskAdr );
		IExec->DebugPrintF( " tn_Parent ...... : %p\n", tn->tn_Parent );
		IExec->DebugPrintF( " tn_Mask ........ : 0x%08lx\n", (U32) tn->tn_Mask );
		/**/ if ( tn->tn_State == TASK_State_Unset )	{ IExec->DebugPrintF( " tn_State ....... : %s", "Unset (0)\n" ); }
		else if ( tn->tn_State == TASK_State_Starting )	{ IExec->DebugPrintF( " tn_State ....... : %s", "Starting (1)\n" ); }
		else if ( tn->tn_State == TASK_State_Running )	{ IExec->DebugPrintF( " tn_State ....... : %s", "Running (2)\n" ); }
		else if ( tn->tn_State == TASK_State_Stopping )	{ IExec->DebugPrintF( " tn_State ....... : %s", "Stoppning (3)\n" ); }
		else if ( tn->tn_State == TASK_State_Stopped )	{ IExec->DebugPrintF( " tn_State ....... : %s", "Stopped (4)\n" ); }
		else											{ IExec->DebugPrintF( " tn_State ....... : 0x%08lx ??\n", (U32) tn->tn_State ); }
		IExec->DebugPrintF( " tn_Type ........ : %ld\n", (S32) tn->tn_Type );
	}

	IExec->Enable();
}
#endif

// --

SEC_CODE void __Task_Settle( struct USBBase *usbbase, struct USB2_TaskNode *tn )
{
U32 timecnt;

	USBDEBUG( "__Task_Settle            : TN    %p :", tn );

	TASKNODE_PRINT(tn);

	if (tn)
	{
		// -- If the Task is Starting/Stopping, give it a little time to settle

		if (( tn->tn_State == TASK_State_Starting ) 
		||	( tn->tn_State == TASK_State_Stopping ))
		{
			USBDEBUG( "__Task_Settle            : Do Settle Delays" );

			timecnt = 0;

			while( TRUE )
			{
				if (( tn->tn_State != TASK_State_Starting ) 
				&&	( tn->tn_State != TASK_State_Stopping ))
				{
					break;
				}

				if ( timecnt >= 25 )
				{
					USBDEBUG( "Process hanging .. timeout reached after %ld", timecnt );
					break;
				}

				timecnt++;

				MISC_WAIT( timecnt * 100000 );	// time * 1/10 sec
			}

			TASKNODE_PRINT(tn);			
		}
		else
		{
			USBDEBUG( "__Task_Settle            : No Settle Needed" );
		}
	}
}

// --
