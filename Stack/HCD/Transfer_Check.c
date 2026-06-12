
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"

// --

/*
** This function checks if the IORequest is done,
** and returns TRUE or FALSE.
**
** If the IORequest is done appropiate removale code is called.
*/

SEC_CODE S32 __HCD_Transfer_Check( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq, U32 Force )
{
struct USB2_EndPointNode *ep;
S32 remove;

	TASK_NAME_ENTER( "HCD : __HCD_Transfer_Check : 1 :" );
	USBDEBUG( "__HCD_Transfer_Check     : IOReq %p : Force %ld", ioreq, Force );

	#ifdef DO_DEBUG

	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "__HCD_Transfer_Check     : Invalid hn %p", hn );
	}

	if ( IOREQUEST_VALID(ioreq) != VSTAT_Okay )
	{
		USBPANIC( "__HCD_Transfer_Check     : Invalid ioreq %p", ioreq );
	}

	#endif

	if ( Force )
	{
		remove = TRUE;
	}
	else
	{
		TASK_NAME_SET( "HCD : __HCD_Transfer_Check : 2" );

		if ( hn->HCD_Functions.Transfer_Check )
		{
			remove = hn->HCD_Functions.Transfer_Check( hn, ioreq );
		}
		else
		{
			USBDEBUG( "__HCD_Transfer_Check     : Transfer_Check : Implement me" );
			remove = FALSE;
		}
	}

	if ( remove )
	{
		TASK_NAME_SET( "HCD : __HCD_Transfer_Check : 3" );

		USBDEBUG( "__HCD_Transfer_Check     : Removing IOReq %p", ioreq );

		// the EP is no longer valid in IOReq after
		// HCD_Remove_Transfer, so get it now
		ep = ioreq->req_EndPoint;

		// --

		#ifdef DO_DEBUG

		if ( ENDPOINT_VALID(ep) != VSTAT_Okay )
		{
			USBPANIC( "__HCD_Transfer_Check     : Invalid ep %p", ep );
		}

		#endif

		// --

		TASK_NAME_SET( "HCD : __HCD_Transfer_Check : 4" );

		// Remove node from Active TransferList
		NODE_REMNODE( & hn->hn_Active_Transfer_List, ioreq );

		TASK_NAME_SET( "HCD : __HCD_Transfer_Check : 5" );

		HCD_TRANSFER_REMOVE( hn, ioreq );

		// Retry on transient errors (CRC/transaction error) if not forced
		// and retries remain. Do NOT retry STALL or BABBLE.
		if (( ! Force )
		&&	( ioreq->req_Public.io_Error == USB2Err_Host_Timeout )
		&&	( ioreq->req_RetryCount < ioreq->req_RetryMax ))
		{
			ioreq->req_RetryCount++;
			ioreq->req_Public.io_Error = 0;

			usbbase->usb_IExec->DebugPrintF( "USB3: Retry %ld/%ld for IOReq %p\n",
				(U32) ioreq->req_RetryCount, (U32) ioreq->req_RetryMax, ioreq );

			// Re-submit: rebuild and re-add to hardware
			HCD_ADD_REQUEST( hn, ioreq );
		}
		else
		{
			TASK_NAME_SET( "HCD : __HCD_Transfer_Check : 6" );

			HCD_REPLY(ioreq);

			TASK_NAME_SET( "HCD : __HCD_Transfer_Check : 7" );

			ioreq = NULL;

			// --
			// Start next IOReq if there are any queued
			HCD_RESTART_ENDPOINT( hn, ep );
		}

		TASK_NAME_SET( "HCD : __HCD_Transfer_Check : 9" );
	}

	TASK_NAME_LEAVE();

	return( remove );
}

// --
