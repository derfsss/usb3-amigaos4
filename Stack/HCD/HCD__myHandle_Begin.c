
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "HCD__Begin_CMD_Read.c"
#include "HCD__Begin_CMD_Write.c"

// --

SEC_CODE static void __myHandle_Begin( struct USBBase *usbbase, struct USB3_HCDNode *hn )
{
struct RealFunctionNode *fn;
struct RealRequest *ioreq;
S32 reply;

	USBDEBUG( "__myHandle_Begin" );
	TASK_NAME_ENTER( "HCD : __myHandle_Begin" );

	while( TRUE )
	{
		ioreq = MSGPORT_GETMSG( & hn->hn_Begin_MsgPort );

		if ( ! ioreq )
		{
			// No more IORequests.. not an error
			break;
		}

		if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
		{
			#ifdef DO_DEBUG

			if ( ioreq->req_PublicStat != IORS_User )
			{
				USBPANIC( "__myHandle_Begin : Invalid HCD IOReq %p Stat ( %ld != %ld )", ioreq, (S32) ioreq->req_PublicStat, (S32) IORS_User );
			}

			#endif

			// HCD is now in control of the IOReq
			ioreq->req_PublicStat = IORS_HCD;

			IOREQUEST_ACTIVE_ADD( ioreq );

			reply = TRUE;

			fn = ioreq->req_Function;

			if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
			{
				SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

				if ( ! fn->fkt_Detach )
				{
					#ifdef DO_DEBUG

					// Must have an Address unless it is 
					// REQCODE_Set_Address of a control ep
					// Hmm okay, I guess RootHub do not need an Address
					if (( fn->fkt_Address == 0 ) && ( fn->fkt_Tier != 0 ))
					{
						struct USB3_EndPointNode *ep = ioreq->req_EndPoint;
						struct USB3_SetupData *sd = ioreq->req_Public.io_SetupData;
						if (( ! sd )
						||	( ! ep )
						||	( ioreq->req_Public.io_SetupLength != sizeof( struct USB3_SetupData ))
						||	( sd->RequestType != ( REQTYPE_Write | REQTYPE_Standard | REQTYPE_Device ))
						||	( sd->RequestCode != REQCODE_Set_Address )
						||	( ep->ep_Type != EPATT_Type_Control ))
						{
							USBDEBUG( "ioreq ......... : $%08lx", ioreq );
							USBDEBUG( "io_SetupData... : $%08lx", ioreq->req_Public.io_SetupData );
							USBDEBUG( "io_SetupLength  : %ld", ioreq->req_Public.io_SetupLength );

							USBDEBUG( "sd ............ : $%08lx", sd );
							if ( sd )
							{
							USBDEBUG( "RequestType ... : $%04lx", LE_SWAP16( sd->RequestType ));
							USBDEBUG( "RequestCode ... : $%04lx", LE_SWAP16( sd->RequestCode ));
							}

							USBDEBUG( "ep ............ : $%08lx", ep );
							if ( ep )
							{
							USBDEBUG( "ep_Type ....... : %ld", ep->ep_Type );
							}

							USBDEBUG( "fn ............ : $%08lx", fn );
							USBDEBUG( "fkt_FreeMe .... : $%04lx", (U32) fn->fkt_FreeMe );
							USBDEBUG( "fkt_Detach .... : $%04lx", (U32) fn->fkt_Detach );
							USBDEBUG( "fkt_Address ... : %ld", fn->fkt_Address );
							USBDEBUG( "fkt_Tier ...... : %ld", fn->fkt_Tier );

							USBPANIC( "__myHandle_Begin : Error FN do not have an Address set" );
						}
					}

					if ( fn->fkt_Address > 127 )
					{
						USBPANIC( "__myHandle_Begin : Invalid Funktion Address #%lu", fn->fkt_Address );
					}

					#endif

					switch( ioreq->req_Public.io_Command )
					{
						case CMD_READ:
						{
							reply = myBegin_CMD_Read( usbbase, hn, ioreq );
							break;
						}

						case CMD_WRITE:
						{
							reply = myBegin_CMD_Write( usbbase, hn, ioreq );
							break;
						}

						default:
						{
							USBDEBUG( "__myHandle_Begin : Unknown Command (%ld)", (S32) ioreq->req_Public.io_Command );
							ioreq->req_Public.io_Error = IOERR_NOCMD;
							reply = TRUE;
							break;
						}
					}
				}
				else
				{
					USBDEBUG( "__myHandle_Begin : Function Detached : FN    %p", fn );
					ioreq->req_Public.io_Error = USB3Err_Device_Detached;
				}

				SEMAPHORE_RELEASE( & fn->fkt_Semaphore );

				FUNCTION_UNLOCK( fn );
			}
			else
			{
				USBDEBUG( "__myHandle_Begin : Invalid Function : FN    %p", fn );
				ioreq->req_Public.io_Error = USB3Err_Stack_InvalidStructure;
			}

			IOREQUEST_UNLOCK( ioreq );

			if ( reply )
			{
				// Give User back control of the IOReq
//				ioreq->req_PublicStat = IORS_User;
//				MSGPORT_REPLYMSG( ioreq );
//				IOREQUEST_ACTIVE_SUB( ioreq );

				HCD_REPLY( ioreq );
				ioreq = NULL;
			}
		}
		else
		{
			// Hmm unknown IORequest type
			USBDEBUG( "__myHandle_Begin : Invalid IOReq %p", ioreq );
			ioreq->req_Public.io_Error = IOERR_NOCMD;
			MSGPORT_REPLYMSG( ioreq );
		}
	}

	TASK_NAME_LEAVE();
}

// --
