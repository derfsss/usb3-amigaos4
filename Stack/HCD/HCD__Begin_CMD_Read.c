
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 myBegin_CMD_Read( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct RealFunctionNode *fn;
struct USB3_EndPointNode *ep;
S32 reply;

	TASK_NAME_ENTER( "HCD : myBegin_CMD_Read" );

	reply = TRUE;		// Default: Yes we have handled the IOReq, lets reply it on exit

//	usbbase->usb_IExec->DebugPrintF( "myBegin_CMD_Read\n" );

	fn = ioreq->req_Function;
	ep = ioreq->req_EndPoint;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBDEBUG( "myBegin_CMD_Read : Device Detached" );
		ioreq->req_Public.io_Error = USB3Err_Device_Detached;
		goto bailout;
	}

	if ( ! ep->ep_Active )
	{
		USBDEBUG( "myBegin_CMD_Read : EndPoint is not Active %ld", ep->ep_Number );
		ioreq->req_Public.io_Error = USB3Err_Stack_EndPointNotFound;
		goto bailout;
	}

	// We need a buffer
	if (( ! ioreq->req_Public.io_Data ) || ( ! ioreq->req_Public.io_Length ))
	{
		// Control can have a Setup buffer instead
		if (( ep->ep_Type != EPATT_Type_Control )
		||	( ! ioreq->req_Public.io_SetupLength )
		||	( ! ioreq->req_Public.io_SetupData ))
		{
			USBDEBUG( "myBegin_CMD_Read : No Buffer found" );
			USBDEBUG( "myBegin_CMD_Read : ioreq ........... : %p",  ioreq );
			USBDEBUG( "myBegin_CMD_Read : ep_Type ......... : %ld", ep->ep_Type );
			USBDEBUG( "myBegin_CMD_Read : io_Data ......... : %p",  ioreq->req_Public.io_Data );
			USBDEBUG( "myBegin_CMD_Read : io_Length ....... : %ld", ioreq->req_Public.io_Length );
			USBDEBUG( "myBegin_CMD_Read : io_SetupData .... : %p",  ioreq->req_Public.io_SetupData );
			USBDEBUG( "myBegin_CMD_Read : io_SetupLength .. : %ld", ioreq->req_Public.io_SetupLength );
			ioreq->req_Public.io_Error = USB3Err_Stack_InvalidStructure;
			goto bailout;
		}
	}

	// -+- Handle IORequest

	if ( ep->ep_Direction & EPDIR_In )
	{
		if ( fn->fkt_Tier == 0 )
		{
			reply = ROOTHUB_HANDLE_READ( hn, ioreq );
		}
		else
		{
			reply = HCD_ADD_REQUEST( hn, ioreq );
		}
	}
	else
	{
		USBDEBUG( "Trying to Send to an inbound EndPoint" );
		ioreq->req_Public.io_Error = USB3Err_Stack_DirectionError;
	}

	// --

bailout:

	TASK_NAME_LEAVE();

	return( reply );
}
