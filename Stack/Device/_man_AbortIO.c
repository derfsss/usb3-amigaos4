
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void _manager_AbortIO( struct DeviceManagerInterface *Self, struct RealRequest *ioreq )
{
struct RealFunctionNode *fn;
struct AbortIOMessage *msg;
struct USB2_HCDNode *hn;
struct USBBase *usbbase;
U32 send;

	TASK_NAME_ENTER( "USB3: _manager_AbortIO" );

	usbbase = (PTR) Self->Data.LibBase;

	#ifdef DO_PANIC

	if ( usbbase->usb_LockSemaphore.us_Semaphore.ss_Owner == usbbase->usb_IExec->FindTask(NULL) )
	{
		USBPANIC( "_manager_AbortIO : You must NOT hold usb_LockSemaphore for this call" );
	}

	#endif

	send = FALSE;
	msg = NULL;

	while( TRUE )
	{
		// --

		msg = MEM_ALLOCVEC( sizeof( struct AbortIOMessage ), TRUE );

//		usbbase->usb_IExec->DebugPrintF( "_manager_AbortIO : Msg %p\n", msg );

		if ( ! msg )
		{
			USBERROR( "_manager_AbortIO : Error allocating memoryp" );
			break;
		}

		// --

//		usbbase->usb_IExec->DebugPrintF( "_manager_AbortIO : IOReq %p\n", ioreq );

		if ( IOREQUEST_LOCK( ioreq ) != LSTAT_Okay )
		{
			USBERROR( "_manager_AbortIO : Ignoreing not a USB2 IORequest" );
			break;
		}

		msg->ioreq = ioreq;

		// --

//		usbbase->usb_IExec->DebugPrintF( "_manager_AbortIO : Stat %ld\n", ioreq->req_PublicStat );

		#ifdef DO_STABLE

		if (( ioreq->req_Detach ) || ( ioreq->req_FreeMe ))
		{
			ioreq->req_Public.io_Error = USB2Err_Stack_InvalidStructure;
			USBPANIC( "_manager_AbortIO : IOReq %p : Invalid IOReq : Detached", ioreq );
			break;
		}

		#endif

		#if 0
		enum IORStat
		{
			IORS_Unset,			// 0 : hmm Error?
			IORS_User,			// 1 : User controlled
			IORS_HCD,			// 2 : Gived to HCD, processing it
			IORS_HCD_Queued,	// 3 : HCD have queued it
			IORS_HCD_Active,	// 4 : HCD have give it to Hardware
		};
		#endif

		if (( ioreq->req_PublicStat == IORS_Unset )
		||	( ioreq->req_PublicStat == IORS_User ))
		{
			USBERROR( "_manager_AbortIO : Not active" );
			break;
		}

		// --

		fn = ioreq->req_Function;

//		usbbase->usb_IExec->DebugPrintF( "_manager_AbortIO : FN    %p\n", fn );

		if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
		{
			USBERROR( "_manager_AbortIO : Not a valid Function" );
			break;	
		}

		msg->Function = fn;

		// --
		// We need to stop IOReq's even if Function is detached
//		usbbase->usb_IExec->DebugPrintF( "_manager_AbortIO : Detach %ld\n", fn->fkt_Detach );

		#if 0

		if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
		{
			USBDEBUG( "_manager_AbortIO : Function Detached" );
			break;
		}

		#endif
		// --

		hn = fn->fkt_HCD;

//		usbbase->usb_IExec->DebugPrintF( "_manager_AbortIO : HN    %p\n", hn );

		if ( HCD_LOCK( hn ) != LSTAT_Okay )
		{
			USBERROR( "_manager_AbortIO : Not a valid HCD" );
			break;	
		}

		msg->HCD = hn;

		// --

		USBINFO( "_manager_AbortIO : Trying to Abort %p IORequst (PUTMSG)", ioreq );

//		usbbase->usb_IExec->DebugPrintF( "_manager_AbortIO : PutMsg %p -> %p\n", msg, & hn->hn_Abort_MsgPort );

		MSGPORT_PUTMSG( & hn->hn_Abort_MsgPort, msg );
		send = TRUE;
		break;
	}

	if (( msg ) && ( ! send ))
	{
		if ( msg->HCD )
		{
			HCD_UNLOCK( msg->HCD );
			msg->HCD = NULL;
		}

		if ( msg->Function )
		{
			FUNCTION_UNLOCK( msg->Function );
			msg->Function = NULL;
		}

		if ( msg->ioreq )
		{
			IOREQUEST_UNLOCK( ioreq );
			msg->ioreq = NULL;
		}

		MEM_FREEVEC( msg );
	}

	TASK_NAME_LEAVE();
}

// --
