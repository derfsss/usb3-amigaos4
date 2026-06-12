
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE U32 __EndPointRes_Destall( struct USBBase *usbbase, struct USB2_EPResource *epr, STR file UNUSED )

#else

SEC_CODE U32 __EndPointRes_Destall( struct USBBase *usbbase, struct USB2_EPResource *epr )

#endif

{
struct RealEndPointResource *real;
struct USB2_EndPointNode *ep;
struct RealFunctionNode *fn;
struct USB2_SetupData sd;
struct RealRequest *ioreq;
U32 retval;
U32 locked;
U32 idx;

	TASK_NAME_ENTER( "__EndPointRes_Destall" );

	USBERROR( "__EndPointRes_Destall       : EPR   %p : (%s)", epr, (file)?file:"<NULL>" );

	retval = FALSE;
	locked = FALSE;
	real = NULL;

	if ( ! epr )
	{
		USBERROR( "__EndPointRes_Destall : NULL Pointer" );
		goto bailout;
	}

	real = (PTR) ( (char *) epr - offsetof( struct RealEndPointResource, epr_Public ));

	if ( ENDPOINTRES_LOCK( real ) != LSTAT_Okay )
	{
		USBERROR( "__EndPointRes_Destall : Error invalid EndPoint Resource (%p)", epr );
		goto bailout;
	}

	locked = TRUE;

	ep = (PTR) real->epr_Public.EndPoint;

	if ( ENDPOINT_VALID( ep ) != VSTAT_Okay )
	{
		USBERROR( "__EndPointRes_Destall : Error validing EndPoint Node" );
		goto bailout;
	}

	fn = ep->ep_Function;

	if ( FUNCTION_VALID( fn ) != VSTAT_Okay )
	{
		USBERROR( "__EndPointRes_Destall : Error validing Function Node" );
		goto bailout;
	}

	// if mp == NULL then alloc IOReq will create the msgport
	ioreq = IOREQUEST_ALLOCTAGS(
		USB2Tag_IOReq_EndPoint, fn->fkt_ControlEndPoint,
		USB2Tag_IOReq_TimeOut, 500000,	 // 0.5 sec timeout
		USB2Tag_IOReq_MsgPort, NULL,	 // Create one for me
		TAG_END
	);

	if ( ! ioreq )
	{
		USBERROR( "__EndPointRes_Destall : Error creating IORequest" );
		goto bailout;
	}

	idx  = ( ep->ep_Direction == EPDIR_Out ) ? EPADR_Dir_Out : EPADR_Dir_In ;
	idx |= ( ep->ep_Number );

	sd.RequestType	= REQTYPE_Write | REQTYPE_Standard | REQTYPE_EndPoint;
	sd.RequestCode	= REQCODE_Clear_Feature;
	sd.Value		= LE_SWAP16( USBSTATUS_EndPoint_Halt );
	sd.Index		= LE_SWAP16( idx );
	sd.Length		= LE_SWAP16( 0 );

	ioreq->req_Public.io_Command		= CMD_WRITE;
	ioreq->req_Public.io_Data			= NULL;
	ioreq->req_Public.io_Length			= 0;
	ioreq->req_Public.io_SetupData		= & sd;
	ioreq->req_Public.io_SetupLength	= sizeof( struct USB2_SetupData );

//	MSGPORT_PUTMSG( & fkt->fkt_HCDNode->hn_Begin_MsgPort, ioreq );
//	MSGPORT_WAIT(   (APTR) ioreq->rr_IOReq.io_Message.mn_ReplyPort );
//	MSGPORT_GETMSG( (APTR) ioreq->rr_IOReq.io_Message.mn_ReplyPort );

	if ( ! IO_DO( ioreq ))
	{
		retval = TRUE;
	}
	else
	{
		USBERROR( "__EndPointRes_Destall : Failed io_Error = %ld", ioreq->req_Public.io_Error );
	}

	IOREQUEST_FREE( ioreq );

bailout:

	if ( locked )
	{
		ENDPOINTRES_UNLOCK( real );
	}

	TASK_NAME_LEAVE();
	return( retval );
}

// --
