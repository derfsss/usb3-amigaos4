
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __myHandle_Reset_Abort( struct USBBase *usbbase, struct USB3_HCDNode *hn )
{
struct USB3_EndPointNode *ep;
struct RealRequest *ioreq;

	TASK_NAME_ENTER( "HCD : __myHandle_Reset_Abort" );

	while( TRUE )
	{
		ioreq = NODE_REMHEAD( & hn->hn_Active_Transfer_List );

		usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.2B - IOReq %p\n", ioreq );

		if ( ! ioreq )
		{
			break;
		}

		// --

		ep = ioreq->req_EndPoint;

		// IOReq and Active should be the same
		usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.2B - ep %p (Active)\n", ep->ep_Request_Active );

		ep->ep_Request_Active = NULL;

		// --
		
		usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.2C - Stop Timer\n" );

		// Stop/Remove timer if started
		if ( ioreq->req_TimerAdded )
		{
			IO_STOP( & ioreq->req_TimerIOReq );
			ioreq->req_TimerAdded = FALSE;
		}

		usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.2D - Transfer Free\n" );

		// Free any buffers allocated
		if ( hn->HCD_Functions.Transfer_Free )
		{
			 hn->HCD_Functions.Transfer_Free( hn, ioreq );
		}

		usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.2E - Reply 1\n" );

		// Send back to user
		ioreq->req_Public.io_Actual = 0;
		ioreq->req_Public.io_Error = USB3Err_Device_Nak;
//		ioreq->req_PublicStat = IORS_User;
//		MSGPORT_REPLYMSG( ioreq );
//		IOREQUEST_ACTIVE_SUB( ioreq );

		HCD_REPLY( ioreq );
		ioreq = NULL;

		// -- The EndPoint may have queued more IORequests

		while( TRUE )
		{
			ioreq = NODE_REMHEAD( & ep->ep_Request_Queue );

//			usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.5A - IOReq %p\n", ioreq );

			if ( ! ioreq )
			{
				break;
			}

			// These are not active only queued, so no Timers or buffers
//			usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.5B - Reply 2\n" );

			// Send back to user
			ioreq->req_Public.io_Actual = 0;
			ioreq->req_Public.io_Error = USB3Err_Device_Nak;
//			ioreq->req_PublicStat = IORS_User;
//			MSGPORT_REPLYMSG( ioreq );
//			IOREQUEST_ACTIVE_SUB( ioreq );

			HCD_REPLY( ioreq );
			ioreq = NULL;
		}
	}

	TASK_NAME_LEAVE();
}

// --

SEC_CODE static void __myHandle_Reset( struct USBBase *usbbase, struct USB3_HCDNode *hn )
{
//	usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1 - Reset\n" );

	if ( hn->HCD_Functions.Chip_Reset )
	{
		if ( ! hn->HCD_Functions.Chip_Reset( hn ))
		{
			usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : Chip_Reset() Failed\n" );
		}
	}

//	usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 1.2A - Transfers (Active)\n" );

	__myHandle_Reset_Abort( usbbase, hn );

//	usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 2 - Deinit\n" );

	if ( hn->HCD_Functions.Chip_Deinit )
	{
		hn->HCD_Functions.Chip_Deinit( hn );
	}

//	usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 3 - Init\n" );

	if ( hn->HCD_Functions.Chip_Init )
	{
		if ( ! hn->HCD_Functions.Chip_Init( hn ))
		{
			usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : Chip_Init() Failed\n" );
		}
	}

//	usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 4 - Start\n" );

	if ( hn->HCD_Functions.Chip_Start )
	{
		if ( ! hn->HCD_Functions.Chip_Start( hn ))
		{
			usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : Chip_Start() Failed\n" );
		}
	}

//	usbbase->usb_IExec->DebugPrintF( "__myHandle_Reset         : 5 - Done\n" );
}

// --
