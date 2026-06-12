
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_InterfaceHeader *ih UNUSED )
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

SEC_CODE enum FSTAT __Interface_FreeHeader( struct USBBase *usbbase, struct USB3_InterfaceHeader *ih, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Interface_FreeHeader( struct USBBase *usbbase, struct USB3_InterfaceHeader *ih )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__Interface_FreeHeader" );

	USBDEBUG( "__Interface_FreeHeader    : IH    %p : (%s)", ih, (file)?file:"<NULL>" );

	// --

	if ( INTERFACE_LOCKHEADER(ih) == LSTAT_Okay )
	{
		if ( ! ih->ih_FreeMe )
		{
			USBINFO( "Marking IH %p FreeMe", ih );
			ih->ih_FreeMe = TRUE;
		}

		__Release( usbbase, ih );

		INTERFACE_UNLOCKHEADER(ih);
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = INTERFACE_VALIDHEADER(ih);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( ih->ih_Locks > 0 )
		{
			USBDEBUG( "__Interface_FreeHeader    : IH    %p : Interface Header still have %ld locks", ih, ih->ih_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			ih->ih_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__Interface_FreeHeader    : IH    %p : Invalid Pointer", ih );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Interface_FreeHeader    : Freeing %p : (%s)", ih, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next(ih)) && ( Node_Next(ih) != (PTR) 0xAC111111 ))
	||	(( Node_Prev(ih)) && ( Node_Prev(ih) != (PTR) 0xAC222222 )))
    {
     	USBPANIC( "__Interface_FreeHeader    : IH    %p : Interface Header have not been removed : Self %p : Next %p : Prev %p : (%s)", ih, Node_Next( ih ), Node_Prev(ih), file );
	}

	#endif

	// --

	if ( ih->ih_Function )
	{
		FUNCTION_UNLOCK( ih->ih_Function );
		ih->ih_Function = NULL;
	}

	// --

	if ( ih->ih_Group )
	{
		INTERFACE_UNLOCKGROUP( ih->ih_Group );
		ih->ih_Group = NULL;
	}

	// --

	MEMORY_FREE( MEMID_USBInterfaceHeader, ih, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
