
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --

SEC_CODE void MSD_Main__myHandle_Abort( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED )
{
	TASK_NAME_ENTER( "MSD : __myHandle_Abort" );
	USBERROR( "MSD : __myHandle_Abort" );

	#if 0
struct USB3_EndPointNode *ep;
struct RealFunctionNode *fn;
struct AbortIOMessage *msg;
struct RealRequest *ioreq2;
struct RealRequest *ioreq;

	while( TRUE )
	{
		msg = MSGPORT_GETMSG( & hn->hn_Abort_MsgPort );

//		usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : Got Msg %p\n", msg );

		if ( ! msg )
		{
			break;
		}

		ioreq = msg->ioreq;

		// --

		USBDEBUG( "__myHandle_Abort         : IOReq %p", ioreq );

//		usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : IOReq %p\n", ioreq );

		if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
		{
			fn = ioreq->req_Function;

			USBDEBUG( "__myHandle_Abort         : FN    %p", fn );

//			usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : FN    %p\n", fn );

			if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
			{
				SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

				ep = ioreq->req_EndPoint;

				USBDEBUG( "__myHandle_Abort         : EP    %p", ep );

//				usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : EP    %p\n", ep );

				if ( ENDPOINT_LOCK(ep) == LSTAT_Okay )
				{
					switch( ioreq->req_Public.io_Command )
					{
						case CMD_READ:
						case CMD_WRITE:
						{
							/**/ if ( ioreq->req_PublicStat == IORS_HCD_Active )
							{
								// IORequest is active
								USBDEBUG( "__myHandle_Abort         : Aborting Active IOReq %p", ioreq );

//								usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : Aborting Active IOReq %p\n", ioreq );

								ioreq->req_Public.io_Error = IOERR_ABORTED;

								HCD_TRANSFER_CHECK( hn, ioreq, TRUE );
							}
							else if ( ioreq->req_PublicStat == IORS_HCD_Queued )
							{
								// IORequest is queued
								ioreq2 = ep->ep_Request_Queue.uh_Head;
		
								while( ioreq2 )
								{
									if ( ioreq2 == ioreq )
									{
										break;
									}
									else
									{
										ioreq2 = Node_Next( ioreq2 );
									}
								}
		
								if ( ioreq2 )
								{
									USBDEBUG( "__myHandle_Abort         : Aborting Queued IOReq %p", ioreq2 );

//									usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : Aborting Queued IOReq %p\n", ioreq2 );

									NODE_REMNODE( & ep->ep_Request_Queue, ioreq2 );

									// Send back to user
									ioreq2->req_Public.io_Error = IOERR_ABORTED;
//									ioreq2->req_PublicStat = IORS_User;
//									MSGPORT_REPLYMSG(ioreq2);
//									IOREQUEST_ACTIVE_SUB( ioreq2 );

									HCD_REPLY( ioreq );
									ioreq2 = NULL;
								}
								else
								{
									USBDEBUG( "__myHandle_Abort         : Queued IOReq %p not found", ioreq );
								}
							}
							else // I guess stat have changed
							{
								USBDEBUG( "__myHandle_Abort         : IOReq %p changed stat %ld", ioreq, ioreq->req_PublicStat );
							}
							break;
						}

						default:
						{
							USBDEBUG( "__myHandle_Abort         : Not a Read/Write command (%ld)", (S32) ioreq->req_Public.io_Command );
							break;
						}
					}

					ENDPOINT_UNLOCK(ep);
				}
				else
				{
					USBDEBUG( "__myHandle_Abort         : Invalid EP %p", ep );
				}

				SEMAPHORE_RELEASE( & fn->fkt_Semaphore );

				FUNCTION_UNLOCK( fn );
			}
			else
			{
				USBDEBUG( "__myHandle_Abort         : Invalid FN %p", fn );
			}

			IOREQUEST_UNLOCK( ioreq );
		}
		else
		{
			USBDEBUG( "__myHandle_Abort         : Invalid IOReq %p", ioreq );
		}

		// --

//		usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : Free Msg %p : 1 :\n", msg );

		#ifdef DO_PANIC
		if ( HCD_VALID( msg->HCD ) == VSTAT_Okay )
		#else
		if ( msg->HCD )
		#endif
		{
			HCD_UNLOCK( msg->HCD );
			msg->HCD = NULL;
		}

		#ifdef DO_PANIC
		if ( FUNCTION_VALID( msg->Function ) == VSTAT_Okay )
		#else
		if ( msg->Function )
		#endif
		{
			FUNCTION_UNLOCK( msg->Function );
			msg->Function = NULL;
		}

		#ifdef DO_PANIC
		if ( IOREQUEST_VALID( msg->ioreq ) == VSTAT_Okay )
		#else
		if ( msg->ioreq )
		#endif
		{
			IOREQUEST_UNLOCK( msg->ioreq );
			msg->ioreq = NULL;
		}

		MEM_FREEVEC( msg );
		msg = NULL;

//		usbbase->usb_IExec->DebugPrintF( "__myHandle_Abort : Free Msg %p : 2 :\n", msg );
	}
	#endif

	TASK_NAME_LEAVE();
}

// --
