
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __HCD_Restart_EndPoint( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct USB2_EndPointNode *ep )
{
struct RealFunctionNode *fn;
struct RealRequest *ioreq;
S32 again;

	TASK_NAME_ENTER( "HCD : __HCD_Restart_EndPoint" );

	/*
	** Restart Next if any is queued
	*/

	again = TRUE;

	while( again )
	{
		again = FALSE;

		// --

		ioreq = NODE_REMHEAD( & ep->ep_Request_Queue );

		if ( ioreq )
		{
			fn = ioreq->req_Function;

			SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

			#ifdef DO_DEBUG

			if ( FUNCTION_VALID(fn) != VSTAT_Okay )
			{
				USBPANIC( "myEHCI_EndPoint_Restart : IOReq %p : Invalid fn %p", ioreq, fn );
			}

			#endif

			if ( HCD_ADD_REQUEST( hn, ioreq ))
			{
//				__HCD_Transfer_Remove( usbbase, hn, ioreq );
				HCD_TRANSFER_REMOVE( hn, ioreq );


//				// Send back to user
//				ioreq->req_PublicStat = IORS_User;
//				MSGPORT_REPLYMSG(ioreq);
//				IOREQUEST_ACTIVE_SUB( ioreq );

				HCD_REPLY(ioreq);
				ioreq = NULL;

				again = TRUE;
			}

			SEMAPHORE_RELEASE( & fn->fkt_Semaphore );
		}
	}

	TASK_NAME_LEAVE();
}

// --
