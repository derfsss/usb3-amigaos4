
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

SEC_CODE struct RealRequest *__IORequest_AllocList( struct USBBase *usbbase, struct TagItem *taglist, STR file UNUSED )

#else

SEC_CODE struct RealRequest *__IORequest_AllocList( struct USBBase *usbbase, struct TagItem *taglist )

#endif

{
struct RealFunctionNode *fn;
struct USB2_EndPointNode *ep;
struct RealRequest *ioreq;
struct MsgPort *mp;
U32 shortp;
U32 addzp;
U32 to;
//U32 sp;
int error;

	USBDEBUG( "__IORequest_AllocList    : (%s)", (file)?file:"<NULL>" );

	ioreq	= NULL;
	error	= TRUE;

	shortp	= MISC_GETTAGVALUE(	USB2Tag_IOReq_AllowShortPackets, TRUE, taglist );
	addzp	= MISC_GETTAGVALUE(	USB2Tag_IOReq_AddZeroPacket,	0, taglist );
	ep		= MISC_GETTAGDATA(	USB2Tag_IOReq_EndPoint,			0, taglist );
	mp		= MISC_GETTAGDATA(	USB2Tag_IOReq_MsgPort,			0, taglist );
	to		= MISC_GETTAGVALUE(	USB2Tag_IOReq_TimeOut,			0, taglist );

	if ( ! ep )
	{
		USBDEBUG( "__IORequest_AllocList    : EndPoint is missing" );
		goto bailout;
	}

	if ( ENDPOINT_VALID(ep) != VSTAT_Okay )
	{
		USBDEBUG( "__IORequest_AllocList    : Invalid EndPoint Pointer" );
		goto bailout;
	}

	fn = ep->ep_Function;

	ioreq = (PTR) MEMORY_ALLOC( MEMID_USBIORequest, TRUE );

	if ( ! ioreq )
	{
		USBERROR( "__IORequest_AllocList    : Error allocating memory" );
		goto bailout;
	}

	ioreq->req_StructID = ID_USB2_IOR;

	if ( ! mp )
	{
		if ( ! MSGPORT_INIT( & ioreq->req_MsgPort ))
		{
			USBERROR( "__IORequest_AllocList    : Error initing MsgPort" );
			goto bailout;
		}

		mp = & ioreq->req_MsgPort.ump_MsgPort;
//		usbbase->usb_IExec->DebugPrintF( "NewMP %p\n", mp );
	}
	else
	{
		USBDEBUG( "__IORequest_AllocList    : Using MsgPort %p", mp );
//		usbbase->usb_IExec->DebugPrintF( "OldMP %p\n", mp );
	}

	/* Public */

//	ioreq->req_Public.io_Message.mn_Node.ln_Succ	= NULL;
//	ioreq->req_Public.io_Message.mn_Node.ln_Pred	= NULL;
	ioreq->req_Public.io_Message.mn_Node.ln_Type	= NT_REPLYMSG;
//	ioreq->req_Public.io_Message.mn_Node.ln_Pri		= 0;
//	ioreq->req_Public.io_Message.mn_Node.ln_Name	= NULL;
	ioreq->req_Public.io_Message.mn_ReplyPort		= mp;
	ioreq->req_Public.io_Message.mn_Length			= sizeof( struct USB2_IORequest );
	ioreq->req_Public.io_Device			= (PTR) usbbase;
	ioreq->req_Public.io_Unit			= (PTR) & usbbase->usb_OpenUnit;
	ioreq->req_Public.io_Command		= ( ep->ep_Direction == EPDIR_Out ) ? CMD_WRITE : CMD_READ;
//	ioreq->req_Public.io_Flags			= 0;
//	ioreq->req_Public.io_Error			= 0;
//	ioreq->req_Public.io_Actual			= 0;
//	ioreq->req_Public.io_Length			= 0;
//	ioreq->req_Public.io_Data			= 0;
//	ioreq->req_Public.io_Offset			= 0;
//	ioreq->req_Public.io_SetupLength	= 0;
//	ioreq->req_Public.io_SetupData		= 0;
//	ioreq->req_Public.io_UserValue		= 0;
//	ioreq->req_Public.io_UserData		= NULL;
//	ioreq->req_Public.io_TimeOut		= 0;
	ioreq->req_Public.io_AllowShortPackets = ( shortp ) ? TRUE : FALSE ;
	ioreq->req_Public.io_AddZeroPackets	= addzp;
	ioreq->req_Public.io_TimeOut		= to;
//	ioreq->req_Public.io_Pad[2];
	ioreq->req_PublicStat				= IORS_User;

	/* Internal */

	// --

	if ( ENDPOINT_LOCK(ep) != LSTAT_Okay )
	{
		USBDEBUG( "__IORequest_AllocList    : EndPoint Lock Error" );
		goto bailout;
	}

	ioreq->req_EndPoint = ep;

	if (( ep->ep_Detach ) || ( ep->ep_FreeMe ))
	{
		USBDEBUG( "__IORequest_AllocList    : Error EndPoint Detached" );
		goto bailout;
	}

	// --

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		USBDEBUG( "__IORequest_AllocList    : Function Lock Error" );
		goto bailout;
	}

	ioreq->req_Function = fn;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBDEBUG( "__IORequest_AllocList    : Error Funtion Detached" );
		goto bailout;
	}

	// --

	//	ioreq->rr_IOReq.io_AllowShortPackets = sp;
	//	ioreq->req_Public.io_TimeOut = to;

//	#ifdef DO_DEBUG
//	ioreq->rr_TaskName = ((struct Task *)TASK_FIND())->tc_Node.ln_Name;
//	#endif

	error = FALSE;

bailout:

	if ((ioreq) && ( error ))
	{
		IOREQUEST_FREE( (PTR) ioreq );
		ioreq = NULL;
	}

	USBDEBUG( "__IORequest_AllocList    : IOReq %p : (new)", ioreq );

	return( ioreq );
}

// --

SEC_CODE struct RealRequest * VARARGS68K __IORequest_AllocTags( struct USBBase *usbbase, ... )
{
struct RealRequest *ioreq;
va_list ap;

//	USBDEBUG( "__IORequest_AllocTags    :" );

	va_start( ap, usbbase );

	ioreq = IOREQUEST_ALLOCLIST( va_getlinearva( ap, struct TagItem * ));

	va_end( ap );

	return( ioreq );
}

// --
