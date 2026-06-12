
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

/*
\\
// Returns:
\\   TRUE  - if Transfer was handled and is finish.
//   FALSE - if Transfer need more time to finish
\\
*/

SEC_CODE static inline S32 myAdd_Request( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
void ( *add )( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
struct USB3_EndPointNode *ep;
struct RealFunctionNode *fn;
S32 handled;
S32 do_req;
S32 err;

	handled = FALSE;

	add = NULL;
	err	= USB3Err_NoError;
	fn	= ioreq->req_Function;
	ep	= ioreq->req_EndPoint;

	// --
	// Endpoint Stall Info:
	// - Control (Setup Stage)	--> Never stalls
	// - Control (Data/Status)	--> Can stall
	// - Bulk					--> Can stall
	// - Interrupt				--> Can stall
	// - Isochronous			--> Never stalls
	//
	// --
	// First figure out if its a Clear Halt (Destall)

	if (( ep->ep_Type == ( EPATT_Type_Control ))
	&&	( ioreq->req_Public.io_Data == 0 )
	&&	( ioreq->req_Public.io_Length == 0 )
	&&	( ioreq->req_Public.io_SetupData )
	&&	( ioreq->req_Public.io_SetupData->RequestType	== ( REQTYPE_Write | REQTYPE_Standard | REQTYPE_EndPoint ))
	&&	( ioreq->req_Public.io_SetupData->RequestCode	== ( REQCODE_Clear_Feature ))
	&&	( ioreq->req_Public.io_SetupData->Value			== ( LE_SWAP16( USBSTATUS_EndPoint_Halt )))
	&&	( ioreq->req_Public.io_SetupLength				== ( sizeof( struct USB3_SetupData ))))
	{
		ioreq->req_DoingDestall = TRUE;

//		#ifdef DO_DEBUG
		U32 index = LE_SWAP16( ioreq->req_Public.io_SetupData->Index );

		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "[%s] Destalling Pipe -- Addr %ld -- EP $%02lx\n", usbbase->usb_IExec->FindTask(NULL)->tc_Node.ln_Name, (U32) fn->fkt_Address, index );
		usbbase->usb_IExec->Enable();
//		#endif
	}
	else
	{
		ioreq->req_DoingDestall = FALSE;
	}

	// Initialize retry tracking for new submissions
	ioreq->req_RetryCount = 0;
	ioreq->req_RetryMax   = 3;

	// --
	// Now figure out if we can execute the Request
	// or maybe the pipe is stalled

	if (( ep->ep_Type == EPATT_Type_Control )
	||	( ep->ep_Type == EPATT_Type_Interrupt )
	||	( ep->ep_Type == EPATT_Type_Bulk ))
	{
		// Check if EP Pipe is stalled
		if (( ep->ep_Stalled ) && ( ! ioreq->req_DoingDestall ))
		{
//			usbbase->usb_IExec->Disable();
//			usbbase->usb_IExec->DebugPrintF( "Pipe is stalled -- Addr %ld -- EP $%02lx\n", (U32) fn->fkt_Address, (U32) ep->ep_Number );
//			usbbase->usb_IExec->Enable();

			handled	= TRUE;
			do_req	= FALSE;
			err		= USB3Err_Host_Stall;
		}
		else
		{
			do_req = TRUE;
		}
	}
	else // ( ep->ep_Type == EPATT_Type_Isohoronous )
	{
		// Can't stall, always true
		do_req = TRUE;
	}

	if ( do_req )
	{
		switch( ep->ep_Type )
		{
			case EPATT_Type_Control: // 0x00
			{
				if ( hn->HCD_Functions.Control_Build )
				{
					handled = hn->HCD_Functions.Control_Build( hn, ioreq );
				}
				else
				{
					USBERROR( "HCD_myRequest_Add : Control_Build : Not Implemented in HCD" );
					handled = TRUE;
					err	= USB3Err_Host_IllegalEPType;
				}

				if ( hn->HCD_Functions.Control_Add )
				{
					add = hn->HCD_Functions.Control_Add;
				}
				else
				{
					USBERROR( "HCD_myRequest_Add : Control_Add : Not Implemented in HCD" );
				}
				break;
			}

//			case EPATT_Type_Isochronous: // 0x01

			case EPATT_Type_Bulk: // 0x02
			{
				if ( hn->HCD_Functions.Bulk_Build )
				{
					handled = hn->HCD_Functions.Bulk_Build( hn, ioreq );
				}
				else
				{
					USBERROR( "HCD_myRequest_Add Bulk_Build : Not Implemented in HCD" );
					handled = TRUE;
					err	= USB3Err_Host_IllegalEPType;
				}

				if ( hn->HCD_Functions.Bulk_Add )
				{
					add = hn->HCD_Functions.Bulk_Add;
				}
				else
				{
					USBERROR( "HCD_myRequest_Add : Bulk_Add : Not Implemented in HCD" );
				}
				break;
			}

			case EPATT_Type_Interrupt: // 0x03
			{
				if ( hn->HCD_Functions.Interrupt_Build )
				{
					handled = hn->HCD_Functions.Interrupt_Build( hn, ioreq );
				}
				else
				{
					USBERROR( "HCD_myRequest_Add : Interrupt_Build : Not Implemented in HCD" );
					handled = TRUE;
					err	= USB3Err_Host_IllegalEPType;
				}

				if ( hn->HCD_Functions.Interrupt_Add )
				{
					add = hn->HCD_Functions.Interrupt_Add;
				}
				else
				{
					USBERROR( "HCD_myRequest_Add : Interrupt_Add : Not Implemented in HCD" );
				}
				break;
			}

			default:
			{
				USBERROR( "HCD_myRequest_Add : Unsupported transfer type %lu -- Implement me", ep->ep_Type );
				handled = TRUE;
				err	= USB3Err_Host_IllegalEPType;
				break;
			}
		}

		if ( ! handled )
		{
			// --
			// We are good, now mark EndPoint as Active
			ep->ep_Request_Active = ioreq;

			// -- 
			// Time to activate it

			ioreq->req_PublicStat = IORS_HCD_Active;
			NODE_ADDTAIL( & hn->hn_Active_Transfer_List, ioreq );

			if ( add )
			{
				add( hn, ioreq );
			}
			else
			{
				USBERROR( "HCD_myRequest_Add : xx_Add : Not Implemented in HCD" );
			}

			// --
			// Start Timer : if requested --

			if ( ioreq->req_Public.io_TimeOut )
			{
				#ifdef DO_DEBUG

				if ( ioreq->req_TimerAdded )
				{
					USBPANIC( "HCD_myRequest_Add : Error TimerAdded == TRUE ( ioreq %p )", ioreq );
				}
				else

				#endif

				MEM_COPY( usbbase->usb_TimeRequest, & ioreq->req_TimerIOReq, sizeof( struct TimeRequest ));

				ioreq->req_TimerIOReq.Request.io_Message.mn_Node.ln_Name = (PTR) ioreq;
				ioreq->req_TimerIOReq.Request.io_Message.mn_ReplyPort = & hn->hn_WatchDog_MsgPort.ump_MsgPort;
				ioreq->req_TimerIOReq.Request.io_Command = TR_ADDREQUEST;
				ioreq->req_TimerIOReq.Time.Microseconds = ioreq->req_Public.io_TimeOut % 1000000;
				ioreq->req_TimerIOReq.Time.Seconds = ioreq->req_Public.io_TimeOut / 1000000;
				ioreq->req_TimerAdded = TRUE;

				IO_SEND( & ioreq->req_TimerIOReq );
			}
		}
		else
		{
			// --
			// Something wrong, Free resources

			if ( hn->HCD_Functions.Transfer_Free )
			{
				hn->HCD_Functions.Transfer_Free( hn, ioreq );
			}
			else
			{
				USBERROR( "HCD_myRequest_Add : Transfer_Free : Not Implemented in HCD" );
			}
		}
	}

	// --
	// Set error code, if not allready set

	if ( ! ioreq->req_Public.io_Error )
	{
		ioreq->req_Public.io_Error = err;
	}

	return( handled );
}

// --

/*
\\
// Returns:
\\   TRUE  - if Transfer was handled and is finish.
//   FALSE - if Transfer need more time to finish
\\
*/

SEC_CODE S32 __HCD_Add_Request( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB3_EndPointNode *ep;
S32 handled;

	TASK_NAME_ENTER( "HCD : __HCD_Add_Request" );

	// --
	// Can we start Request or do we queue it

	ep = ioreq->req_EndPoint;

	if ( ep->ep_Request_Active )
	{
		USBDEBUG( "Queuing Node for Addr $%02ld : EP    $%02lx", ioreq->req_Function->fkt_Address, ep->ep_Number );

		ioreq->req_PublicStat = IORS_HCD_Queued;
		NODE_ADDTAIL( & ep->ep_Request_Queue, ioreq );

		handled = FALSE;
	}
	else
	{
		handled = myAdd_Request( usbbase, hn, ioreq );
	}

	TASK_NAME_LEAVE();

	return( handled );
}

// --
