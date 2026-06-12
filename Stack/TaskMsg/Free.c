
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
// Release all resource we can, for this Struct

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_TaskMsg *tm )
{
struct USB3_TaskNode *tn;

//	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
	// --

	tn = tm->tm_Task;

	if (tn)
	{
		tm->tm_Task = NULL;
		// tn_Owner do not belong to us
		TASK_UNLOCK(tn);
	}

	// --
//	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

/*
** The Node, should have been Removed from the list header before calling.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __TaskMsg_Free( struct USBBase *usbbase, struct USB3_TaskMsg *tm, STR file UNUSED )

#else

SEC_CODE enum FSTAT __TaskMsg_Free( struct USBBase *usbbase, struct USB3_TaskMsg *tm )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__TaskMsg_Free" );

	USBDEBUG( "__TaskMsg_Free            : TM %p : (%s)", tm, (file)?file:"<NULL>" );

	// --

	if ( TASKMSG_LOCK(tm) == LSTAT_Okay )
	{
		if ( ! tm->tm_FreeMe )
		{
			USBINFO( "Marking TM %p FreeMe", tm );
			tm->tm_FreeMe = TRUE;
		}

		__Release( usbbase, tm );

		TASKMSG_UNLOCK(tm);
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = TASKMSG_VALID(tm);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if (( tm->tm_Locks > 0 ) || ( tm->tm_Task ))
		{
			USBDEBUG( "__TaskMsg_Free            : TaskMsg Node still have %ld locks", tm->tm_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			tm->tm_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__TaskMsg_Free            : Invalid Pointer (%p) : (%s)", tm, file );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		USBDEBUG( "__TaskMsg_Free            : 123 : Can't Freeing %p", tm );
		goto bailout;
	}

	USBINFO( "__TaskMsg_Free            : Freeing %p : (%s)", tm, (file)?file:"<NULL>" );

	// --

//	#ifdef DO_PANIC
//
//	Okay this is used in a MsgPort.. so it do not get Removed via Node_xx
//	if ((( Node_Next(tm)) && ( Node_Next(tm) != (PTR) 0xAC111111 ))
//	||	(( Node_Prev(tm)) && ( Node_Prev(tm) != (PTR) 0xAC222222 )))
//	{
//		USBPANIC( "__TaskMsg_Free            : TaskMsg Node have not been removed : Self %p : Next %p : Prev %p : (%s)", tm, Node_Next(tm), Node_Prev(tm), file );
//	}
//
//	#endif

	// --

//	if ( tm->tm_Function )
//	{
//		FUNCTION_UNLOCK( tm->tm_Function );
//		tm->tm_Function = NULL;
//	}

	// --

	if ( tm->tm_HCD )
	{
		HCD_UNLOCK( tm->tm_HCD );
		tm->tm_HCD = NULL;
	}

	// --

//	USBDEBUG( "__TaskMsg_Free            : 22 : DN %p : Cmd %ld", tm->tm_Driver, tm->tm_Cmd.cm_Command );

//	if ( tm->tm_Driver )
//	{
//		// Signal HCD if Stop Process and a Driver
//		if ( tm->tm_Cmd.cm_Command == CMD_STOP_PROCESS )
//		{
//			USBDEBUG( "__TaskMsg_Free            : 33 : Signal HCD" );
//
//			HCD_SIGNAL_DRIVERNODE( tm->tm_Driver );
//		}
//
//		USBDEBUG( "__TaskMsg_Free            : 44 : Unlock DN" );
//
//		DRIVER_UNLOCK( tm->tm_Driver );
//		tm->tm_Driver = NULL;		
//	}

	// --

//	USBDEBUG( "__TaskMsg_Free            : 55 : Free Mem" );

	MEMORY_FREE( MEMID_USBTaskMsg, tm, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
