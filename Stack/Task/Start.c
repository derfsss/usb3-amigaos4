
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE enum TaskReturn __Task_Start( struct USBBase *usbbase, ... )
{
struct USB3_TaskMsg *tm;
struct USB3_HCDNode *hn;
enum TaskReturn retval;
va_list ap;
U32 data;
U32 tag;

	retval = TASK_Return_Stack_Error;

//	USBDEBUG( "__Task_Start : 1 : " );

	va_start( ap, usbbase );

	hn = NULL;
	tm = TASKMSG_ALLOC();

	if ( ! tm )
	{
		USBERROR( "__Task_Start : 2 : Error allocating TaskMsg" );
		goto bailout;
	}

	tm->tm_Cmd.cm_Command = CMD_START_PROCESS;

	while( TRUE )
	{
		tag = va_arg( ap, U32 );

		if ( ! tag )
		{
			break;
		}

		data = va_arg( ap, U32 );

		switch( tag )
		{
			case TASK_Tag_Func_Init:
			{
				tm->tm_Func_Init = (PTR) data ;

				USBDEBUG( "TASK_Tag_Func_Init ..... : %p", (PTR) data );
				break;
			}

			case TASK_Tag_Func_Main:
			{
				tm->tm_Func_Entry = (PTR) data ;

				USBDEBUG( "TASK_Tag_Func_Entry .... : %p", (PTR) data );
				break;
			}

			case TASK_Tag_Func_Free:
			{
				tm->tm_Func_Free = (PTR) data ;

				USBDEBUG( "TASK_Tag_Func_Free ..... : %p", (PTR) data );
				break;
			}

			case TASK_Tag_Prioity:
			{
				tm->tm_Prioity = (S32) data ;

				USBDEBUG( "TASK_Tag_Prioity ....... : %ld", (S32) data );
				break;
			}

			case TASK_Tag_ASync:
			{
				tm->tm_ASync = (PTR) data ;

				USBDEBUG( "TASK_Tag_ASync ......... : %p", (PTR) data );
				break;
			}

			case TASK_Tag_UserData:
			{
				tm->tm_UserData = (PTR) data ;

				USBDEBUG( "TASK_Tag_UserData ...... : %p", (PTR) data );
				break;
			}

			case TASK_Tag_Name:
			{
				tm->tm_Name = (STR) data ;

				USBDEBUG( "TASK_Tag_Name .......... : %s", (STR) data );
				break;
			}

			case TASK_Tag_Type:
			{
				tm->tm_Type = data ;

				USBDEBUG( "TASK_Tag_Type .......... : %ld", (S32) data );
				break;
			}

			case TASK_Tag_HCDNode:
			{
				hn = (PTR) data;

				USBDEBUG( "TASK_Tag_HCDNode ....... : %p", (PTR) data );
				break;
			}

			case TASK_Tag_Get_TN:
			{
				tm->tm_Get_TaskNode = (PTR) data;

				USBDEBUG( "TASK_Tag_Get_TN ........ : %p", (PTR) data );
				break;
			}

			case USB3_WaitForTermination:
			{
				tm->tm_Wait_For_Term = ( data ) ? 1 : 0 ;

				USBDEBUG( "USB3_WaitForTermination  : %ld", (S32) data );
				break;
			}

			case TASK_Tag_InternSize:
			{
				tm->tm_InternSize = data;

				USBDEBUG( "TASK_Tag_InternSize .... : %lu bytes", (U32) data );
				break;
			}

			default:
			{
				USBDEBUG( "Unknown Tag ............ : [%08lx:%08lx]", (U32) tag, (U32) data );
				break;
			}
		}
	}

	if (( ! tm->tm_Func_Init ) && ( ! tm->tm_Func_Entry ) && ( ! tm->tm_Func_Free ))
	{
		USBDEBUG( "__Task_Start : 4 : Missing Process Entry Point" );
		goto bailout;
	}

	if (( tm->tm_Type <= TASK_Type_Unset ) || ( tm->tm_Type >= TASK_Type_Last ))
	{
		USBDEBUG( "__Task_Start : 5 : Invalid Process Type (%ld)", tm->tm_Type );
		goto bailout;
	}

	// --

	if ( hn )
	{
		if ( HCD_LOCK( hn ) != LSTAT_Okay )
		{
			USBDEBUG( "__Task_Start : Invalid HCD Node (%p)", hn );
			goto bailout;
		}

		tm->tm_HCD = hn;
	}

	// --

	retval = MISC_DOCOMMAND( & tm->tm_Cmd );

	// --

bailout:

	#ifdef DO_DEBUG
	if ( ! IS_TASKRETURN(retval) )
	{
		USBPANIC( "TaskReturn value error : %08lx", (U32) retval );
	}
	#endif

	va_end( ap );

	if (tm)
	{
		USBDEBUG( "__Task_Start : 99 : Free TM %p", tm );
		TASKMSG_FREE(tm);
	}

	return( retval );
}

// --