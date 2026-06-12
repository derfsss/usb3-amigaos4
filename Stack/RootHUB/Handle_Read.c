
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "__Ctrl_Read.c"

// --

SEC_CODE S32 __RootHUB_Handle_Read( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
S32 reply;

	reply = TRUE;		// Default: Yes we have handled the IOReq, lets reply it on exit

	USBDEBUG( "__RootHUB_Handle_Read" );

	switch( ioreq->req_EndPoint->ep_Number )
	{
		case 0:
		case 16:
		{
			__Control_Read( usbbase, hn, ioreq );
			break;
		}

		case HUB_IntEp_Nr:
		{
//			usbbase->usb_IExec->DebugPrintF( "Queue 2 : %p\n", ioreq );
			NODE_ADDTAIL( & hn->hn_HUB_Interrupts, ioreq );

			// Tigger a PortChange so already connected Device get Reconized
			TASK_SIGNAL( hn->hn_Task->tn_TaskAdr, hn->hn_HUB_Signal.sig_Signal_Mask );

			reply = FALSE;
			break;
		}

		default:
		{
			USBDEBUG( "Root HUB : Unknown EndPoint Nr %lu", (U32) ioreq->req_EndPoint->ep_Number );

			ioreq->req_Public.io_Error = USB3Err_Host_Stall;
			break;
		}
	}

	return( reply );
}

// --
