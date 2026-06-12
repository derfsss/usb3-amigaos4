
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

SEC_CODE static void myFree_TNs( struct USBBase *usbbase )
{
struct USB2_TaskNode *next;
struct USB2_TaskNode *tn;

//	USBDEBUG( "myFree_TNs :: Enter" );

	SEMAPHORE_OBTAIN( & usbbase->usb_Task_Semaphore );

	tn = usbbase->usb_Task_Header.uh_Head;

	while( tn )
	{
		next = Node_Next(tn);

		if (( tn->tn_TaskAdr == NULL )
		&&	( tn->tn_Locks <= 0 )
		&&	( tn->tn_State == TASK_State_Stopped ))
		{
			NODE_REMNODE( & usbbase->usb_Task_Header, tn );

			if ( TASK_FREE(tn) != FSTAT_Okay )
			{
				USBDEBUG( "myFree_TNs :: Unable to Free %p", tn );
				TASKNODE_PRINT(tn);
				NODE_ADDHEAD( & usbbase->usb_Task_Header, tn );
			}
			else
			{
//				usbbase->usb_IExec->DebugPrintF( "\n##\n## Task Node %p freed\n##\n\n", tn );
			}
		}

		tn = next;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Task_Semaphore );

//	USBDEBUG( "myFree_TNs :: Leave" );
}

// --
