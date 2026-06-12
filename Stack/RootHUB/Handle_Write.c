
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "__Ctrl_Write.c"

// --

SEC_CODE S32 __RootHUB_Handle_Write( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
S32 reply;

	reply = TRUE;		// Default: Yes we have handled the IOReq, lets reply it on exit

	USBDEBUG( "__RootHUB_Handle_Write" );

	switch( ioreq->req_EndPoint->ep_Number )
	{
		case 0:
		case 16:
		{
			__Control_Write( usbbase, hn, ioreq );
			break;
		}

		default:
		{
			USBDEBUG( "Root HUB : Unknown EndPoint Nr %lu", (U32) ioreq->req_EndPoint->ep_Number );
			ioreq->req_Public.io_Error = USB2Err_Host_Stall;
			break;
		}
	}

	return( reply );
}

// --
