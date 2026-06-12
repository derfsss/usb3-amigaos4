
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

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct RealRequest *ioreq UNUSED )
{
//	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
	// --


	// --
//	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __IORequest_Free( struct USBBase *usbbase, struct RealRequest *ioreq, STR file UNUSED )

#else

SEC_CODE enum FSTAT __IORequest_Free( struct USBBase *usbbase, struct RealRequest *ioreq )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__IORequest_Free" );

	USBDEBUG( "__IORequest_Free         : IOReq %p : (%s)", ioreq, (file)?file:"<NULL>" );

	// --

	if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
	{
		if ( ! ioreq->req_FreeMe )
		{
			USBINFO( "Marking IOReq %p FreeMe", ioreq );
			ioreq->req_FreeMe = TRUE;
		}

		__Release( usbbase, ioreq );

		IOREQUEST_UNLOCK( ioreq );
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = IOREQUEST_VALID(ioreq);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
//		usbbase->usb_IExec->DebugPrintF( "__IORequest_Free         : IOReq %p : Locks %2ld : Stat %ld : Task %p :\n", ioreq, ioreq->req_Locks, ioreq->req_PublicStat, usbbase->usb_IExec->FindTask(NULL) );

		if (( ioreq->req_PublicStat != IORS_User )
		&&	( ioreq->req_PublicStat != IORS_Unset ))
		{
			usbbase->usb_IExec->DebugPrintF( "__IORequest_Free         : IOReq %p : Node still active (Stat %ld)\n", ioreq, ioreq->req_PublicStat );
			USBDEBUG( "__IORequest_Free         : IOReq %p : Node still active (Stat %ld)", ioreq, ioreq->req_PublicStat );
			fstat = FSTAT_Locked;
		}
		else if ( ioreq->req_Locks > 0 )
		{
			usbbase->usb_IExec->DebugPrintF( "__IORequest_Free         : IOReq %p : Node still have %ld locks\n", ioreq, ioreq->req_Locks );
			USBDEBUG( "__IORequest_Free         : IOReq %p : Node still have %ld locks", ioreq, ioreq->req_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			ioreq->req_StructID = ID_USB2_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__IORequest_Free    : Invalid Pointer (%p)", ioreq );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__IORequest_Free         : Freeing %p : (%s)", ioreq, (file)?file:"<NULL>" );

	// --

//	#ifdef DO_PANIC
//
//	Okay this is used in a MsgPort.. so it do not get Removed via Node_xx
//	if ((( Node_Next(ioreq)) && ( Node_Next(ioreq) != (PTR) 0xAC111111 ))
//	||	(( Node_Prev(ioreq)) && ( Node_Prev(ioreq) != (PTR) 0xAC222222 )))
//	{
//		USBPANIC( "__IORequest_Free    : Register Node have not been removed : Self %p : Next %p : Prev %p : (%s)", ioreq, Node_Next(ioreq), Node_Prev(ioreq), file );
//	}
//
//	#endif

	// --

	#if 0

	if ( ioreq->rr_StructID != StructID_IOR )
	{
		USBDEBUG( "Invalid Request ID" );
		goto bailout;
	}

	#ifdef DO_DEBUG
	if (( ioreq->rr_HCDActive ) || ( ioreq->rr_HCDControlled ))
	{
		USBDEBUG( "IORequest is still Active" );
	}

	if ( ioreq->rr_TimerAdded )
	{
		USBDEBUG( "IORequest's Timer is still Active" );
	}
	#endif

	if (( ioreq->rr_MsgPortUsed ) && ( ioreq->rr_IOReq.io_Message.mn_ReplyPort ))
	{
		MSGPORT_FREE( & ioreq->rr_MsgPort );
		ioreq->rr_IOReq.io_Message.mn_ReplyPort = NULL;
		ioreq->rr_MsgPortUsed = FALSE;
	}

	#endif

	if ( ioreq->req_EndPoint )
	{
		ENDPOINT_UNLOCK( ioreq->req_EndPoint );
		ioreq->req_EndPoint = NULL;
	}

	if ( ioreq->req_Function )
	{
		FUNCTION_UNLOCK( ioreq->req_Function );
		ioreq->req_Function = NULL;
	}

//	usbbase->usb_IExec->DebugPrintF( "FreeMP %p\n", & ioreq->req_MsgPort.ump_MsgPort );

	MSGPORT_FREE( & ioreq->req_MsgPort );

	MEMORY_FREE( MEMID_USBIORequest, ioreq, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}
