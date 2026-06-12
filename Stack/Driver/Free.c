
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_DriverNode *dn UNUSED )
{
struct USB3_TaskNode *tn;

//	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
	// --

	if ( dn->dn_Task )
	{
		tn = dn->dn_Task;

		if ( tn->tn_Owner == dn )
		{
			TASK_UNLOCK(tn);
			dn->dn_Task = NULL;
			tn->tn_Owner = NULL;
		}
		else
		{
			USBDEBUG( "__Driver_Free : TN    and DN mismatch" );
		}
	}

	// --
//	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

/*
** The Node, should have been Removed from the list header before calling.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __Driver_Free( struct USBBase *usbbase, struct USB3_DriverNode *dn, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Driver_Free( struct USBBase *usbbase, struct USB3_DriverNode *dn )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__Driver_Free" );

	USBDEBUG( "__Driver_Free             : DN    %p : (%s)", dn, (file)?file:"<NULL>" );

	// --

	if ( DRIVER_LOCK( dn ) == LSTAT_Okay )
	{
		if ( ! dn->dn_FreeMe )
		{
			USBINFO( "Marking DN %p FreeMe", dn );
			dn->dn_FreeMe = TRUE;
		}

		__Release( usbbase, dn );

		DRIVER_UNLOCK( dn );
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = DRIVER_VALID( dn );

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if (( dn->dn_Locks > 0 ) || ( dn->dn_Task ))
		{
			#ifdef DO_DEBUG
			if ( dn->dn_Task )
			{
				USBPANIC( "Warning: DriverNode still have a Task %p : DN    %p : (%s)", dn->dn_Task, dn, (file)?file:"<NULL>" );
			}
			#endif

			USBDEBUG( "__Driver_Free             : Driver Node still have %ld locks", dn->dn_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			dn->dn_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__Driver_Free             : Invalid Pointer (%p)", dn );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Driver_Free             : Freeing %p : (%s)", dn, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next( dn )) && ( Node_Next( dn ) != (PTR) 0xAC111111 ))
	||	(( Node_Prev( dn )) && ( Node_Prev( dn ) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__Driver_Free             : Driver Node have not been removed : Self %p : Next %p : Prev %p : (%s)", dn, Node_Next( dn ), Node_Prev( dn ), file );
	}

	#endif

	// --

	if ( dn->dn_HCD )
	{
		HCD_UNLOCK( dn->dn_HCD );
		dn->dn_HCD = NULL;
	}

	// --

	if ( dn->dn_Function )
	{
		FUNCTION_UNLOCK( dn->dn_Function );
		dn->dn_Function = NULL;
	}

	// --

	MEMORY_FREE( MEMID_USBDriver, dn, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
