
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_InterfaceGroup *ig UNUSED )
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

SEC_CODE enum FSTAT __Interface_FreeGroup( struct USBBase *usbbase, struct USB3_InterfaceGroup *ig, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Interface_FreeGroup( struct USBBase *usbbase, struct USB3_InterfaceGroup *ig )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__Interface_FreeGroup" );

	USBDEBUG( "__Interface_FreeGroup     : IG    %p : (%s)", ig, (file)?file:"<NULL>" );

	// --

	if ( INTERFACE_LOCKGROUP(ig) == LSTAT_Okay )
	{
		if ( ! ig->ig_FreeMe )
		{
			USBINFO( "Marking IG %p FreeMe", ig );
			ig->ig_FreeMe = TRUE;
		}

		__Release( usbbase, ig );

		INTERFACE_UNLOCKGROUP(ig);
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = INTERFACE_VALIDGROUP(ig);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( ig->ig_Locks > 0 )
		{
			USBDEBUG( "__Interface_FreeGroup     : Interface Group still have %ld locks", ig->ig_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			ig->ig_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__Interface_FreeGroup : Invalid Pointer (%p)", ig );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Interface_FreeGroup     : Freeing %p : (%s)", ig, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next(ig)) && ( Node_Next(ig) != (PTR) 0xAC111111 ))
	||	(( Node_Prev(ig)) && ( Node_Prev(ig) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__Interface_FreeGroup : Interface Node have not been removed : Self %p : Next %p : Prev %p : (%s)", ig, Node_Next( ig ), Node_Prev(ig), file );
	}

	#endif

	// --

	if ( ig->ig_Function )
	{
		FUNCTION_UNLOCK( ig->ig_Function );
		ig->ig_Function = NULL;
	}

	if ( ig->ig_Config )
	{
		CONFIG_UNLOCK( ig->ig_Config );
		ig->ig_Config = NULL;
	}

	// --

	MEMORY_FREE( MEMID_USBInterfaceGroup, ig, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
