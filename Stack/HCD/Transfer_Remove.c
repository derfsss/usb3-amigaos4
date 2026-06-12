
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
** This function removes the IORequest from active chip lists.
**
** Note: Must be running in the HCD stack context, make sure AbortIO obays this
*/

SEC_CODE void __HCD_Transfer_Remove( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB3_EndPointNode *destall_ep;
struct USB3_EndPointNode *ep;
struct RealFunctionNode *fn;
U32 actual;
U32 offset;
U32 index;

//	USBDEBUG( "__HCD_Transfer_Remove :" );
	TASK_NAME_ENTER( "HCD : __HCD_Transfer_Remove" );

	// --

	fn = ioreq->req_Function;
	ep = ioreq->req_EndPoint;

	#ifdef DO_PANIC

	if ( FUNCTION_VALID( fn ) != VSTAT_Okay )
	{
		USBPANIC( "__HCD_Transfer_Remove : Invalid fn %p", fn );
	}

	if ( ENDPOINT_VALID( ep ) != VSTAT_Okay )
	{
		USBPANIC( "__HCD_Transfer_Remove : Invalid ep %p", ep );
	}

	#endif

	// --

	/*
	** Abort Timer IO Request 
	*/

	if ( ioreq->req_TimerAdded )
	{
		IO_STOP( & ioreq->req_TimerIOReq );
		ioreq->req_TimerAdded = FALSE;
	}

	// --
	// Check if a Destall succeded

	if ( ioreq->req_DoingDestall )
	{
		ioreq->req_DoingDestall = FALSE;
		index = LE_SWAP16( ioreq->req_Public.io_SetupData->Index );

		if (( ioreq->req_PublicStat == IORS_HCD_Active )
		&&	( ioreq->req_Public.io_Error == USB3Err_NoError ))
		{
			offset = ( fn->fkt_Address * 32 ) + ( index & 0x0f );

			if (( index & EPADR_Dir_Mask ) == EPADR_Dir_Out )
			{
				offset += 16;
			}

			destall_ep = hn->hn_EndPoint_Array[ offset ];

			if ( destall_ep )
			{
				usbbase->usb_IExec->DebugPrintF( "[%s] We successfully Destalled -- Addr %ld : EP    $%02lx : EPN Info : EPNr %ld : EPDir %ld : EPSize %ld : EPType %ld\n", 
					usbbase->usb_IExec->FindTask(NULL)->tc_Node.ln_Name, 
					fn->fkt_Address, 
					index,
					destall_ep->ep_Number,
					destall_ep->ep_Direction,
					destall_ep->ep_MaxPacketSize,
					destall_ep->ep_Type
				);

				destall_ep->ep_DataToggle = 0;
				destall_ep->ep_Stalled = FALSE;
			}
			else
			{
				USBERROR( "Destall Error : EndPoint Not Found : Offset %ld", offset );
			}
		}
		else
		{
			usbbase->usb_IExec->DebugPrintF( "[%s] Destalled Failed -- Addr %ld -- EP $%02lx\n", usbbase->usb_IExec->FindTask(NULL)->tc_Node.ln_Name, fn->fkt_Address, index );
		}
	}

	// --
	// Remove Active IOReq

	if ( ioreq->req_PublicStat == IORS_HCD_Active )
	{
		// EHCI's DoorBell need the HCD Context

		switch( ep->ep_Type )
		{
			case EPATT_Type_Control:
			{
				if ( hn->HCD_Functions.Control_Remove )
				{
					hn->HCD_Functions.Control_Remove( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Control_Remove not Implemented in HCD" );
				}
				break;
			}

			case EPATT_Type_Interrupt:
			{
				if ( hn->HCD_Functions.Interrupt_Remove )
				{
					hn->HCD_Functions.Interrupt_Remove( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Interrupt_Remove not Implemented in HCD" );
				}
				break;
			}

			case EPATT_Type_Bulk:
			{
				if ( hn->HCD_Functions.Bulk_Remove )
				{
					USBDEBUG( "__HCD_Transfer_Remove : Bulk_Remove" );
					hn->HCD_Functions.Bulk_Remove( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Bulk_Remove not Implemented in HCD" );
				}
				break;
			}

			case EPATT_Type_Isochronous:
			{
				if ( hn->HCD_Functions.Isochronous_Remove )
				{
					USBDEBUG( "__HCD_Transfer_Remove : Isochronous_Remove" );
					hn->HCD_Functions.Isochronous_Remove( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Isochronous_Remove not Implemented in HCD" );
				}
				break;
			}

			default:
			{
				USBDEBUG( "__HCD_Transfer_Remove : Invalid ep_Type %ld", (S32) ep->ep_Type );
				break;
			}
		}

		// hmm Functions may not be implemented.. 
		// so clear those or should I ??
		// They are after all still active
		ioreq->req_PublicStat = IORS_HCD;
		ep->ep_Request_Active = NULL;
	}

	// --

	/*
	** Count transfered bytes and if a Read command copy data to user buffer
	*/

	actual = 0;

	if ( ioreq->req_Public.io_Error == USB3Err_NoError )
	{
		switch( ep->ep_Type )
		{
			case EPATT_Type_Control:
			{
				if ( hn->HCD_Functions.Control_Length )
				{
					actual = hn->HCD_Functions.Control_Length( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Control_Length not Implemented in HCD" );
				}
				break;
			}

			case EPATT_Type_Interrupt:
			{
				if ( hn->HCD_Functions.Interrupt_Length )
				{
					actual = hn->HCD_Functions.Interrupt_Length( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Interrupt_Length not Implemented in HCD" );
				}
				break;
			}

			case EPATT_Type_Bulk:
			{
				if ( hn->HCD_Functions.Bulk_Length )
				{
					USBDEBUG( "__HCD_Transfer_Remove : Bulk_Length" );
					actual = hn->HCD_Functions.Bulk_Length( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Bulk_Length not Implemented in HCD" );
				}
				break;
			}

			case EPATT_Type_Isochronous:
			{
				if ( hn->HCD_Functions.Isochronous_Length )
				{
					USBDEBUG( "__HCD_Transfer_Remove : Isochronous_Length" );
					actual = hn->HCD_Functions.Isochronous_Length( hn, ioreq );
				}
				else
				{
					USBERROR( "__HCD_Transfer_Remove : Isochronous_Length not Implemented in HCD" );
				}
				break;
			}

			default:
			{
				USBERROR( "__HCD_Transfer_Remove : Invalid ep_Type %ld", (S32) ep->ep_Type );
				break;
			}
		}
	}

	ioreq->req_Public.io_Actual = actual;

	// --
	// Set stall flags, if needed

	if ( ioreq->req_Public.io_Error == USB3Err_Host_Stall )
	{
		ep->ep_Stalled = TRUE;
	}

	// --

	/*
	** We are done now, release resources
	*/

	if ( hn->HCD_Functions.Transfer_Free )
	{
		hn->HCD_Functions.Transfer_Free( hn, ioreq );
	}
	else
	{
		USBERROR( "__HCD_Transfer_Remove : Transfer_Free not Implemented in HCD" );
	}

	// --

	TASK_NAME_LEAVE();
}

// --
