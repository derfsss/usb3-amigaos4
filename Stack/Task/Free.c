
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB2_TaskNode *tn UNUSED )
{
//	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
	// --


	// --
//	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

/*
** The Node, should have been Removed from the list header before calling.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __Task_Free( struct USBBase *usbbase, struct USB2_TaskNode *tn, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Task_Free( struct USBBase *usbbase, struct USB2_TaskNode *tn )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__Task_Free" );

	USBDEBUG( "__Task_Free               : TN %p : (%s)", tn, (file)?file:"<NULL>" );

	// --

	if ( TASK_LOCK(tn) == LSTAT_Okay )
	{
		if ( ! tn->tn_FreeMe )
		{
			USBINFO( "Marking TN %p FreeMe", tn );
			tn->tn_FreeMe = TRUE;
		}

		__Release( usbbase, tn );

		TASK_UNLOCK(tn);
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = TASK_VALID(tn);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if (( tn->tn_Locks > 0 ) || ( tn->tn_TaskAdr ))
		{
			#ifdef DO_DEBUG
			if ( tn->tn_TaskAdr )
			{
				USBPANIC( "Warning: DriverNode still have a Task %p : TN %p :", tn->tn_TaskAdr, tn );
			}
			#endif

			USBDEBUG( "__Task_Free               : Driver Node still have %ld locks", tn->tn_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			tn->tn_StructID = ID_USB2_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__Task_Free               : Invalid Pointer (%p)", tn );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Task_Free               : Freeing %p : (%s)", tn, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next(tn)) && ( Node_Next(tn) != (PTR) 0xAC111111 ))
	||	(( Node_Prev(tn)) && ( Node_Prev(tn) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__Task_Free               : Task Node have not been removed : Self %p : Next %p : Prev %p : (%s)", tn, Node_Next(tn), Node_Prev(tn), file );
	}

	#endif

	// --

	if ( tn->tn_ASync )
	{
		ASYNC_SUB(tn->tn_ASync);
		tn->tn_ASync = NULL;
	}

	// --

	MEMORY_FREE( MEMID_USBTask, tn, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
