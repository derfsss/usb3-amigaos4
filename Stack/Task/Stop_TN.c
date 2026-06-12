
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void __Task_Stop_TN( struct USBBase *usbbase, struct USB3_TaskNode *tn, STR file UNUSED )

#else

SEC_CODE void __Task_Stop_TN( struct USBBase *usbbase, struct USB3_TaskNode *tn )

#endif

{
struct USB3_TaskMsg *tm;

	USBINFO( "__Task_Stop_TN           :  1 : TN %p : (%s)", tn, (file)?file:"<NULL>" );

	if ( TASK_VALID( tn ) == VSTAT_Okay )
	{
		if ( TASK_LOCK( tn ) == LSTAT_Okay )
		{
			// User/Stack want to stop task
			tm = TASKMSG_ALLOC();

			if ( tm )
			{
				TASK_SETTLE( tn );

				tm->tm_Cmd.cm_Command = CMD_STOP_PROCESS;
				tm->tm_Task = tn;

				USBINFO( "__Task_Stop_TN           :  2 : Send PMsg %p", tm );

				MISC_DOCOMMAND( & tm->tm_Cmd );

				if ( tm->tm_Keep )
				{
					USBINFO( "__Task_Stop_TN           :  3 : Keep TM Msg" );
				}
				else
				{
					USBINFO( "__Task_Stop_TN           :  4 : Free Msg : TM %p", tm );
					TASKMSG_FREE( tm );
				}
			}
			else
			{
				USBERROR( "__Task_Stop_TN           :  5 : Error allocating message" );
				TASK_UNLOCK( tn );
			}
		}
		else
		{
			USBINFO( "__Task_Stop_TN           :  6 : Invalid TaskNode %p : (%s)", tn, (file)?file:"<NULL>" );
		}
	}

	USBINFO( "__Task_Stop_TN           : 99 : TN %p", tn );
}

// --
