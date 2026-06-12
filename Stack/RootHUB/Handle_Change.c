
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "__Int_Read.c"

// --

SEC_CODE void __RootHUB_Handle_Chg( struct USBBase *usbbase, struct USB3_HCDNode *hn )
{
struct RealRequest *ioreq;
struct RealRequest *next;
S32 len;

	USBDEBUG( "__RootHUB_Handle_Chg     : Enter" );

	ioreq = hn->hn_HUB_Interrupts.uh_Head;

	if ( ! ioreq )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: Handle_Change: NO IORequests in queue (HCD #%ld)\n", hn->hn_HCDIndex );
	}

	while( ioreq )
	{
		next = Node_Next( ioreq );
		// --

		len = __Interrupt_Read( usbbase, hn, ioreq->req_Public.io_Data, ioreq->req_Public.io_Length );

		usbbase->usb_IExec->DebugPrintF( "USB3: Handle_Change: IntRead len=%ld (HCD #%ld)\n", len, hn->hn_HCDIndex );

		if ( len > 0 )
		{
			NODE_REMNODE( & hn->hn_HUB_Interrupts, ioreq );

			ioreq->req_Public.io_Actual = len;
			ioreq->req_Public.io_Error = USB3Err_NoError;

			HCD_REPLY( ioreq );
			ioreq = NULL;
		}

		// --
		ioreq = next;
	}

	USBDEBUG( "__RootHUB_Handle_Chg     : Leave" );
}

// --

