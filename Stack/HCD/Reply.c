
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void __HCD_Reply( struct USBBase *usbbase UNUSED, struct RealRequest *ioreq, STR file UNUSED )

#else

SEC_CODE void __HCD_Reply( struct USBBase *usbbase UNUSED, struct RealRequest *ioreq )

#endif

{
	TASK_NAME_ENTER( "__HCD_Reply : 1" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
	{
//		usbbase->usb_IExec->DebugPrintF( "__HCD_Reply              : IOReq %p : Locks %2ld : Stat %ld : Task %p :\n", ioreq, ioreq->req_Locks, ioreq->req_PublicStat, usbbase->usb_IExec->FindTask(NULL) );

		TASK_NAME_SET( "__HCD_Reply : 2" );

//		usbbase->usb_IExec->DebugPrintF( "MP %p\n", ioreq->req_Public.io_Message.mn_ReplyPort );

//		MSGPORT_REPLYMSG( ioreq );
		usbbase->usb_IExec->ReplyMsg( (PTR) & ioreq->req_Public );

		TASK_NAME_SET( "__HCD_Reply : 4" );

		IOREQUEST_ACTIVE_SUB( ioreq );

		usbbase->usb_IExec->Disable();

		TASK_NAME_SET( "__HCD_Reply : 5" );

		// Send back to user
		ioreq->req_PublicStat = IORS_User;

		TASK_NAME_SET( "__HCD_Reply : 3" );

		IOREQUEST_UNLOCK( ioreq );
		ioreq = NULL;

		usbbase->usb_IExec->Enable();
	}
	else
	{
		USBPANIC( "Failed to lock : IOReq %p : (%s)", ioreq, (file)?file:"<NULL>" );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
