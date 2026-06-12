
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

SEC_CODE void __IORequest_Unlock( struct USBBase *usbbase, struct RealRequest *ioreq, STR file UNUSED )

#else

SEC_CODE void __IORequest_Unlock( struct USBBase *usbbase, struct RealRequest *ioreq )

#endif

{
	TASK_NAME_ENTER( "__IORequest_Unlock" );

	USBDEBUG( "__IORequest_Unlock       : IOReq %p : Locks %2ld (old) : (%s)", ioreq, (ioreq)?ioreq->req_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( IOREQUEST_VALID(ioreq) == VSTAT_Okay )
	{
		if ( ioreq->req_Locks > 0 )
		{
			ioreq->req_Locks--;

//			USBDEBUG( "__IORequest_Unlock       : IOReq %p : New Lock Count : %ld (-1)", ioreq, ioreq->req_Locks );
		}
		else
		{
			USBPANIC( "__IORequest_Unlock       : IOReq %p : IORequest NOT Locked (%ld)", ioreq, ioreq->req_Locks );
		}
	}
	else
	{
		USBERROR( "__IORequest_Unlock       : IOReq %p : Invalid IORequest", ioreq );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --

