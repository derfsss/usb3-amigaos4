
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __myHandle_WatchDog( struct USBBase *usbbase, struct USB2_HCDNode *hn )
{
struct RealFunctionNode *fn;
struct RealRequest *ioreq;
struct Node *n;

	TASK_NAME_ENTER( "HCD : __myHandle_WatchDog" );

	// Handle Any IORequest in MsgPort List, they should be done
	while(( n = MSGPORT_GETMSG( & hn->hn_WatchDog_MsgPort )))
	{
		ioreq = (PTR) n->ln_Name;

		#ifdef DO_DEBUG

		if ( IOREQUEST_VALID(ioreq) != VSTAT_Okay )
		{
			USBPANIC( "__myHandle_WatchDog : Invalid IOReq %p", ioreq );
		}

		#endif

		USBDEBUG( "IORequest %p timeout", ioreq );

		fn = ioreq->req_Function;

		#ifdef DO_DEBUG

		if ( FUNCTION_VALID(fn) != VSTAT_Okay )
		{
			USBPANIC( "__myHandle_WatchDog : Invalid Function %p", fn );
		}

		#endif

		SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

		USBERROR( "1345 timeout : timeout val %lu", ioreq->req_Public.io_TimeOut );
		ioreq->req_Public.io_Error = USB2Err_Host_Timeout;

		HCD_TRANSFER_CHECK( hn, ioreq, TRUE );

		SEMAPHORE_RELEASE( & fn->fkt_Semaphore );
	}

	TASK_NAME_LEAVE();
}

// --
