
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __Stopping( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct intern *in )
{
U32 mask;
U32 wait;

	USBINFO( "__myMain (HCD)           : __Stopping (3) ::" );
	TASK_NAME_ENTER( "HCD Main : __Stopping" );

	wait =	SIGBREAKF_CTRL_D |
			hn->HCD_Mask |
			hn->HCD_Restart_Chip.sig_Signal_Mask |
			hn->hn_HUB_Signal.sig_Signal_Mask |
			hn->hn_Abort_MsgPort.ump_Signal.sig_Signal_Mask |
			hn->hn_Begin_MsgPort.ump_Signal.sig_Signal_Mask |
			hn->hn_WatchDog_MsgPort.ump_Signal.sig_Signal_Mask |
			in->Tick_MsgPort.ump_Signal.sig_Signal_Mask ;

	while( TRUE )
	{
		// -- Check possible Exit
		// For now we only checks for Drivers started .. only HUB driver? right
		// I guess this can grow at some point

		// dev_Task_Header is for all Stack Tasks (minus Master)

		#if 0
		USBDEBUG( 
			"__myMain (HCD)\n"
			" :: Index ...... : #%ld\n"
			" :: Stopping ... :\n"
			" :: 3 .......... :\n"
			" :: Tasks ...... : %ld\n"
			" :: HUB ASync .. : %ld\n"
			" ::\n", 
			hn->hn_HCDIndex, 
			usbbase->usb_Task_Header.uh_Nodes, 
			hn->hn_HUB_ASync.ua_Counter 
		);
		#endif

		if ( hn->hn_HUB_ASync.ua_Counter <= 0 )
		{
			USBDEBUG( "__myMain (HCD)           : Index #%ld :: Wee can stop now ::", hn->hn_HCDIndex );
			break;
		}

		// --

		mask = TASK_WAIT( wait );

		// Lets put it first
		if ( mask & hn->HCD_Restart_Chip.sig_Signal_Mask )
		{
			__myHandle_Reset( usbbase, hn );
			mask = 0;
		}

		if ( mask & hn->HCD_Mask )
		{
			usbbase->usb_IExec->DebugPrintF( " : __myHandle_HCD\n" );

			__myHandle_HCD( usbbase, hn, mask );
		}

		if ( mask & hn->hn_WatchDog_MsgPort.ump_Signal.sig_Signal_Mask )
		{
//			USBPANIC( "__myMain (HCD) : __Stopping : hn_WatchDog_MsgPort : Implement me" );
			usbbase->usb_IExec->DebugPrintF( " : __myHandle_WatchDog\n" );

			__myHandle_WatchDog( usbbase, hn );
		}

		if ( mask & hn->hn_Abort_MsgPort.ump_Signal.sig_Signal_Mask )
		{
			usbbase->usb_IExec->DebugPrintF( " : __myHandle_Abort\n" );

			__myHandle_Abort( usbbase, hn );
		}

		if ( mask & hn->hn_Begin_MsgPort.ump_Signal.sig_Signal_Mask )
		{
			usbbase->usb_IExec->DebugPrintF( " : __myHandle_Begin\n" );

			__myHandle_Begin( usbbase, hn );
		}

		if ( mask & hn->hn_HUB_Signal.sig_Signal_Mask )
		{
			ROOTHUB_HANDLE_CHANGE( hn );
		}

		if ( mask & in->Tick_MsgPort.ump_Signal.sig_Signal_Mask )
		{
			USBDEBUG( "__myMain (HCD)           : Got : Tick Timer :" );

			if ( MSGPORT_GETMSG( & in->Tick_MsgPort ))
			{
//				myFree_DNs( usbbase );
//				myFree_TNs( usbbase );
				in->Tick_TimeRequest.Time.Seconds = 1;
				in->Tick_TimeRequest.Time.Microseconds = 0;
				IO_SEND( & in->Tick_TimeRequest );
			}

			__myHandle_HCD( usbbase, hn, -1 );
		}

		if ( mask & SIGBREAKF_CTRL_D )
		{
			USBDEBUG( "__myMain (HCD)           : Got : CTRL+D :" );
			// Used to break out of Wait so we can check ua_Counter
		}
	}

	TASK_NAME_LEAVE();
}

// --
