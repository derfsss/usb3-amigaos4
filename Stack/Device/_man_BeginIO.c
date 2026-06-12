
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

#include "NSCmd_DeviceQuery.c"

// --

SEC_CODE void IOREQUEST_PRINT( struct USBBase *usbbase, struct RealRequest *ioreq )
{
struct ExecIFace *IExec;

	IExec = usbbase->usb_IExec;

	IExec->Disable();
	IExec->DebugPrintF( "IORequest : ioreq %p\n", ioreq );
	IExec->DebugPrintF( "req_StructID ..... : $%08lx\n", ioreq->req_StructID );
	IExec->DebugPrintF( "req_Locks ........ : %ld\n", ioreq->req_Locks );
	IExec->DebugPrintF( "req_Detach ....... : %ld\n", ioreq->req_Detach );
	IExec->DebugPrintF( "req_Function ..... : %p\n", ioreq->req_Function );
	IExec->DebugPrintF( "req_EndPoint ..... : %p\n", ioreq->req_EndPoint );
	IExec->DebugPrintF( "req_PublicStat .... : %ld\n", (S32) ioreq->req_PublicStat );
	IExec->Enable();

	#if 0
	struct USB3_IORequest			req_Public;
	// -- 
	U32								req_StructID;
	S32								req_Locks;
	U16								req_Detach;
	U16								req_FreeMe;
//	struct USB3_TaskNode *			req_Task;
//	struct USB3_ASync *				req_ASync;
	// --
	struct RealFunctionNode *		req_Function;
	struct USB3_EndPointNode *		req_EndPoint;
	struct USB3_MsgPort				req_MsgPort;
//	struct TimeRequest				req_TimerIOReq;
	enum IORStat					req_PublicStat;
	#endif

}


static void _manager_BeginIO( struct DeviceManagerInterface *Self, struct RealRequest *ioreq )
{
struct RealFunctionNode *fn;
struct USB3_HCDNode *hn;
//struct ExecIFace *IExec;
struct USBBase *usbbase;
S32 reply;

	TASK_NAME_ENTER( "USB3: _manager_BeginIO" );

	usbbase = (PTR) Self->Data.LibBase;

//	IExec = usbbase->usb_IExec;
//	IExec->DebugPrintF( "\n_manager_BeginIO\n" );

	#ifdef DO_PANIC

	if ( usbbase->usb_LockSemaphore.us_Semaphore.ss_Owner == usbbase->usb_IExec->FindTask(NULL) )
	{
		USBPANIC( "_manager_BeginIO : You must NOT hold usb_LockSemaphore for this call" );
	}

	#endif

	ioreq->req_Public.io_Message.mn_Node.ln_Type = NT_MESSAGE;
	ioreq->req_Public.io_Flags	= 0;
	ioreq->req_Public.io_Error	= 0;
	ioreq->req_Public.io_Actual	= 0;

	reply = TRUE;

	if ( IOREQUEST_VALID(ioreq) == VSTAT_Okay )
	{
		while( TRUE )
		{
			if ( ioreq->req_PublicStat != IORS_User )
			{
				IOREQUEST_PRINT( usbbase, ioreq );

				ioreq->req_Public.io_Error = USB3Err_Stack_InvalidStructure;
				USBPANIC( "_manager_BeginIO : IOReq %p : Invalid IOReq : Stat %ld != %ld", ioreq, (S32) ioreq->req_PublicStat, (S32) IORS_User );
				break;
			}

			#ifdef DO_STABLE

			if (( ioreq->req_Detach ) || ( ioreq->req_FreeMe ))
			{
				ioreq->req_Public.io_Error = USB3Err_Stack_InvalidStructure;
				USBPANIC( "_manager_BeginIO : IOReq %p : Invalid IOReq : Detached", ioreq );
				break;
			}

			#endif

			fn = ioreq->req_Function;

			if ( FUNCTION_VALID(fn) != VSTAT_Okay )
			{
				ioreq->req_Public.io_Error = USB3Err_Stack_InvalidStructure;
				USBDEBUG( "_manager_BeginIO : Invalid Function Node : FN    %p", fn );
				break;
			}

			if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
			{
				ioreq->req_Public.io_Error = USB3Err_Device_Detached;
				USBDEBUG( "_manager_BeginIO : Function Detached" );
				break;
			}

			hn = fn->fkt_HCD;

			if ( HCD_VALID( hn ) != VSTAT_Okay )
			{
				ioreq->req_Public.io_Error = USB3Err_Stack_InvalidStructure;
				USBDEBUG( "_manager_BeginIO : Invalid hn Node" );
				break;
			}

//			usbbase->usb_IExec->DebugPrintF( "Queue 1 : %p\n", ioreq );
			MSGPORT_PUTMSG( & hn->hn_Begin_MsgPort, ioreq );
			reply = FALSE;
			break;
		}

		if ( reply )
		{
			ioreq->req_PublicStat = IORS_User;
			MSGPORT_REPLYMSG( ioreq );
		}
	}
	else
	{
		switch( ioreq->req_Public.io_Command )
		{
			case NSCMD_DEVICEQUERY:
			{
				NSCmd_DeviceQuery( usbbase, (PTR) ioreq );
				break;
			}

			default:
			{
				// Unknown Command
				USBDEBUG( "_manager_BeginIO : Unknown Command (%04lx)", ioreq->req_Public.io_Command );
				ioreq->req_Public.io_Error = IOERR_NOCMD;
				break;
			}
		}

		MSGPORT_REPLYMSG( ioreq );
	}

	TASK_NAME_LEAVE();
}
