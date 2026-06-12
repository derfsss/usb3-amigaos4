
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB2_FktDriverNode *fdn UNUSED )
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

SEC_CODE enum FSTAT __FDriver_Free( struct USBBase *usbbase, struct USB2_FktDriverNode *fdn, STR file UNUSED )

#else

SEC_CODE enum FSTAT __FDriver_Free( struct USBBase *usbbase, struct USB2_FktDriverNode *fdn )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__FDriver_Free" );

	USBDEBUG( "__FDriver_Free           : FDN %p : (%)", fdn, (file)?file:"<NULL>" );

	// --

	if ( FDRIVER_LOCK( fdn ) == LSTAT_Okay )
	{
		if ( ! fdn->fdn_FreeMe )
		{
			USBINFO( "Marking FDN %p FreeMe", fdn );
			fdn->fdn_FreeMe = TRUE;
		}

		__Release( usbbase, fdn );

		FDRIVER_UNLOCK( fdn );
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = FDRIVER_VALID( fdn );

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( fdn->fdn_Locks > 0 )
		{
			USBDEBUG( "__FDriver_Free  : FDriver Node still have %ld locks", fdn->fdn_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			fdn->fdn_StructID = ID_USB2_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__FDriver_Free  : Invalid Pointer (%p)", fdn );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__FDriver_Free           : Freeing %p : (%s)", fdn, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next( fdn )) && ( Node_Next( fdn ) != (PTR) 0xAC111111 ))
	||	(( Node_Prev( fdn )) && ( Node_Prev( fdn ) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__FDriver_Free  : FDriver Node have not been removed : Self %p : Next %p : Prev %p : (%s)", fdn, Node_Next( fdn ), Node_Prev( fdn ), file );
	}

	#endif

	// --

	if ( fdn->fdn_Title )
	{
		MEM_FREEVEC( fdn->fdn_Title );
		fdn->fdn_Title = NULL;
	}

	// --

	MEMORY_FREE( MEMID_USBFktDriver, fdn, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
