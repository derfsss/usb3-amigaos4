
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_EndPointNode *ep UNUSED )
{
struct RealRequest *ioreq;
struct RealRequest *next;

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
	// --

	USBDEBUG( "Sending Abort : EP    : %p : check", ep );

	ioreq = ep->ep_Request_Queue.uh_Head;

	while( ioreq )
	{
		next = Node_Next( ioreq );
		// --

		USBDEBUG( "Sending Abort : EP    : %p : IOReq %p (queue)", ep, ioreq );

		if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
		{
			SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
			IO_ABORT( ioreq );
			SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
			IOREQUEST_UNLOCK( ioreq );
		}

		// --
		ioreq = next;
	}

	// --

	ioreq = ep->ep_Request_Active;

	if ( ioreq )
	{
		USBDEBUG( "Sending Abort : EP    : %p : IOReq %p (active)", ep, ioreq );

		if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
		{
			SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
			IO_ABORT( ioreq );
			SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
			IOREQUEST_UNLOCK( ioreq );
		}
	}

	// --
	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

/*
** The Node, should have been Removed from the list header before calling.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __EndPoint_Free( struct USBBase *usbbase, struct USB3_EndPointNode *ep, STR file UNUSED )

#else

SEC_CODE enum FSTAT __EndPoint_Free( struct USBBase *usbbase, struct USB3_EndPointNode *ep )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__EndPoint_Free" );

	USBDEBUG( "__EndPoint_Free           : EP    %p : (%s)", ep, (file)?file:"<NULL>" );

	// --

	if ( ENDPOINT_LOCK(ep) == LSTAT_Okay )
	{
		if ( ! ep->ep_FreeMe )
		{
			USBINFO( "Marking EP %p FreeMe", ep );
			ep->ep_FreeMe = TRUE;
		}

		struct RealFunctionNode *fn = ep->ep_Function;
		if ( ! fn->fkt_ActiveIOReq )
		{
//			usbbase->usb_IExec->DebugPrintF( "FN %p : fkt_ActiveIOReq %ld : Freeing\n", fn, fn->fkt_ActiveIOReq );
			__Release( usbbase, ep );
		}
		else
		{
			usbbase->usb_IExec->DebugPrintF( "EP %p : FN    %p : fkt_ActiveIOReq %ld : Skipping\n", ep, fn, fn->fkt_ActiveIOReq );
		}

		ENDPOINT_UNLOCK(ep);
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = ENDPOINT_VALID(ep);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( ep->ep_Locks > 0 )
		{
			USBDEBUG( "__EndPoint_Free           : EndPoint Node still have %ld locks", ep->ep_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			ep->ep_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__EndPoint_Free           : Invalid Pointer (%p)", ep );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__EndPoint_Free           : Freeing %p : (%s)", ep, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next(ep)) && ( Node_Next(ep) != (PTR) 0xAC111111 ))
	||	(( Node_Prev(ep)) && ( Node_Prev(ep) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__EndPoint_Free  : EndPoint Node have not been removed : Self %p : Next %p : Prev %p : (%s)", ep, Node_Next( ep ), Node_Prev(ep), file );
	}

	#endif

	// --

	if ( ep->ep_Interface )
	{
		INTERFACE_UNLOCKNODE( ep->ep_Interface );
		ep->ep_Interface = NULL;
	}

	if ( ep->ep_Function )
	{
		FUNCTION_UNLOCK( ep->ep_Function );
		ep->ep_Function = NULL;
	}

	// --

	MEMORY_FREE( MEMID_USBEndPoint, ep, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
