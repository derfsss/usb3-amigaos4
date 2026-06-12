
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __Free_3_Misc( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct intern *in )
{
	// -- Timer Tick

	if ( in->Tick_TimerAdded )
	{
		in->Tick_TimerAdded = FALSE;
		IO_STOP( & in->Tick_TimeRequest );
	}

	MSGPORT_FREE( & in->Tick_MsgPort );

	// --

	USBDEBUG( ":: HN    ASync Wait :: 11 :: Count %ld", hn->hn_HUB_ASync.ua_Counter );
	ASYNC_WAIT( & hn->hn_HUB_ASync );
//	USBDEBUG( ":: HN    ASync Wait :: 22 :: Count %ld", hn->hn_HUB_ASync.ua_Counter );
	ASYNC_FREE( & hn->hn_HUB_ASync );
	USBDEBUG( ":: HN    ASync Wait :: 33 :: Count %ld", hn->hn_HUB_ASync.ua_Counter );

//	USBDEBUG( ":: HN    :: AA :: hn_WatchDog_MsgPort" );
	MSGPORT_FREE( & hn->hn_WatchDog_MsgPort );

//	USBDEBUG( ":: HN    :: BB :: hn_Delay_MsgPort" );
	MSGPORT_FREE( & hn->hn_Delay_MsgPort );

//	USBDEBUG( ":: HN    :: CC :: hn_Begin_MsgPort" );
	MSGPORT_FREE( & hn->hn_Begin_MsgPort );

//	USBDEBUG( ":: HN    :: DD :: hn_Abort_MsgPort" );
	MSGPORT_FREE( & hn->hn_Abort_MsgPort );

//	USBDEBUG( ":: HN    :: EE2 :: HCD_Restart_Chip" );
	TASK_FREESIGNAL( & hn->HCD_Restart_Chip );

//	USBDEBUG( ":: HN    :: EE1 :: hn_HUB_Signal" );
	TASK_FREESIGNAL( & hn->hn_HUB_Signal );

//	USBDEBUG( ":: HN    :: FF :: hn_HUB_Desc" );
	MEM_FREEVEC( hn->hn_HUB_Desc );
	hn->hn_HUB_Desc = NULL;

//	USBDEBUG( ":: HN    :: GG :: hn_EndPoint_Array" );
	MEM_FREEVEC( hn->hn_EndPoint_Array );
	hn->hn_EndPoint_Array = NULL;

//	USBDEBUG( ":: HN    :: HH :: hn_Function_Array" );
	MEM_FREEVEC( hn->hn_Function_Array );
	hn->hn_Function_Array = NULL;
};

// --
