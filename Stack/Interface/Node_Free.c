
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_InterfaceNode *in UNUSED )
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

SEC_CODE enum FSTAT __Interface_FreeNode( struct USBBase *usbbase, struct USB3_InterfaceNode *in, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Interface_FreeNode( struct USBBase *usbbase, struct USB3_InterfaceNode *in )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__Interface_FreeNode" );

	USBDEBUG( "__Register_Free          : IN    %p : (%s)", in, (file)?file:"<NULL>" );

	// --

	if ( INTERFACE_LOCKNODE(in) == LSTAT_Okay )
	{
		if ( ! in->in_FreeMe )
		{
			USBINFO( "Marking IN %p FreeMe", in );
			in->in_FreeMe = TRUE;
		}

		__Release( usbbase, in );

		INTERFACE_UNLOCKNODE(in);
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = INTERFACE_VALIDNODE( in );

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( in->in_Locks > 0 )
		{
			USBDEBUG( "__Interface_FreeNode      : Interface Node still have %ld locks", in->in_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			in->in_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__Interface_FreeNode      : Invalid Pointer (%p)", in );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Interface_FreeNode      : Freeing %p : (%s)", in, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next( in )) && ( Node_Next( in ) != (PTR) 0xAC111111 ))
	||	(( Node_Prev( in )) && ( Node_Prev( in ) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__Interface_FreeNode      : Interface Node have not been removed : Self %p : Next %p : Prev %p : (%s)", in, Node_Next(in), Node_Prev(in), file );
	}

	#endif

	// --

	if ( in->in_Parent )
	{
		INTERFACE_UNLOCKHEADER( in->in_Parent );
		in->in_Parent = NULL;
	}

	// --

	if ( in->in_Function )
	{
		FUNCTION_UNLOCK( in->in_Function );
		in->in_Function = NULL;
	}

	MEMORY_FREE( MEMID_USBInterfaceNode, in, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
