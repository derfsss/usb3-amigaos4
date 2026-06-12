
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_ConfigNode *cn UNUSED )
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

SEC_CODE enum FSTAT __Config_Free( struct USBBase *usbbase, struct USB3_ConfigNode *cn, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Config_Free( struct USBBase *usbbase, struct USB3_ConfigNode *cn )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__Config_Free" );

//	USBDEBUG( "__Config_Free             : CN    %p : (%s)", cn, (file)?file:"<NULL>" );

	// --

	if ( CONFIG_LOCK(cn) == LSTAT_Okay )
	{
		if ( ! cn->cfg_FreeMe )
		{
			USBINFO( "Marking CN %p FreeMe", cn );
			cn->cfg_FreeMe = TRUE;
		}

		__Release( usbbase, cn );

		CONFIG_UNLOCK(cn);
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = CONFIG_VALID(cn);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( cn->cfg_Locks > 0 )
		{
			USBDEBUG( "__Config_Free             : Config Node still have %ld locks", cn->cfg_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			cn->cfg_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "Config_Free  : Invalid Pointer (%p)", cn );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Config_Free             : Freeing %p : (%s)", cn, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next(cn)) && ( Node_Next(cn) != (PTR) 0xAC111111 ))
	||	(( Node_Prev(cn)) && ( Node_Prev(cn) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__Config_Free             : Config Node have not been removed : Self %p : Next %p : Prev %p : (%s)", cn, Node_Next(cn), Node_Prev(cn), file );
	}

	#endif

	// --

	if ( cn->cfg_Function )
	{
		FUNCTION_UNLOCK( cn->cfg_Function );
		cn->cfg_Function = NULL;
	}

	// --

	MEMORY_FREE( MEMID_USBConfig, cn, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
