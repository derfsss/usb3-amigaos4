
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __Normal( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct intern *in )
{
struct USB3_TaskNode *tn;
U32 mask;
U32 wait;

	tn = hn->hn_Task;

	USBDEBUG( "__myMain (HCD)           : __Normal (1) :: HN %p : IN %p : TN %p :", hn, in, tn );
	TASK_NAME_ENTER( "HCD Main : __Normal" );

	wait =	SIGBREAKF_CTRL_C |
			SIGBREAKF_CTRL_D |
			hn->HCD_Mask |
			hn->HCD_Restart_Chip.sig_Signal_Mask |
			hn->hn_HUB_Signal.sig_Signal_Mask |
			hn->hn_Abort_MsgPort.ump_Signal.sig_Signal_Mask |
			hn->hn_Begin_MsgPort.ump_Signal.sig_Signal_Mask |
			hn->hn_WatchDog_MsgPort.ump_Signal.sig_Signal_Mask |
			in->Tick_MsgPort.ump_Signal.sig_Signal_Mask ;

	while( TRUE )
	{
//		USBDEBUG( "__myMain (HCD)           : __Normal :: Index #%ld :: Mask $%08lx :: Tasks %ld :: HUB ASync %ld ::", 
//			hn->hn_HCDIndex, mask, usbbase->usb_Task_Header.uh_Nodes, hn->hn_HUB_ASync.ua_Counter );

		mask = TASK_WAIT( wait );

//		usbbase->usb_IExec->DebugPrintF( "HCD Mask $%08lx\n", mask );

		// Lets put it first
		if ( mask & hn->HCD_Restart_Chip.sig_Signal_Mask )
		{
			__myHandle_Reset( usbbase, hn );
			mask = 0;
		}

		if ( mask & hn->HCD_Mask )
		{
//			usbbase->usb_IExec->DebugPrintF( " : __myHandle_HCD\n" );

			__myHandle_HCD( usbbase, hn, mask );
		}

		if ( mask & hn->hn_WatchDog_MsgPort.ump_Signal.sig_Signal_Mask )
		{
//			usbbase->usb_IExec->DebugPrintF( " : __myHandle_WatchDog\n" );

			__myHandle_WatchDog( usbbase, hn );
		}

		if ( mask & hn->hn_Abort_MsgPort.ump_Signal.sig_Signal_Mask )
		{
//			usbbase->usb_IExec->DebugPrintF( " : __myHandle_Abort\n" );

			__myHandle_Abort( usbbase, hn );
		}

		if ( mask & hn->hn_Begin_MsgPort.ump_Signal.sig_Signal_Mask )
		{
//			usbbase->usb_IExec->DebugPrintF( " : __myHandle_Begin\n" );

			__myHandle_Begin( usbbase, hn );
		}

		if ( mask & hn->hn_HUB_Signal.sig_Signal_Mask )
		{
			usbbase->usb_IExec->DebugPrintF( "USB3: HCD#%ld: hn_HUB_Signal fired\n", hn->hn_HCDIndex );

			ROOTHUB_HANDLE_CHANGE( hn );
		}

		if ( mask & in->Tick_MsgPort.ump_Signal.sig_Signal_Mask )
		{
//			usbbase->usb_IExec->DebugPrintF( " : sig_Signal_Mask\n" );

			if ( MSGPORT_GETMSG( & in->Tick_MsgPort ))
			{
				in->Tick_TimeRequest.Time.Seconds = 1;
				in->Tick_TimeRequest.Time.Microseconds = 0;
				IO_SEND( & in->Tick_TimeRequest );
			}

			__myHandle_HCD( usbbase, hn, -1 );
		}

		if ( mask & SIGBREAKF_CTRL_C )
		{
//			usbbase->usb_IExec->DebugPrintF( " : SIGBREAKF_CTRL_C\n" );

			if ( tn->tn_Parent )
			{
//				usbbase->usb_IExec->DebugPrintF( "\n" );
				USBINFO( "__myMain (HCD)           : Normal :: Index #%ld :: Exit Signal", hn->hn_HCDIndex );
				break;
			}
			else
			{
				USBINFO( "__myMain (HCD)           : Normal :: Index #%ld :: Someone tryed to stop us, ignoring", hn->hn_HCDIndex );
			}
			break;
		}

		if ( mask & SIGBREAKF_CTRL_D )
		{
//			usbbase->usb_IExec->DebugPrintF( " : SIGBREAKF_CTRL_D" );

			USBDEBUG( "__myMain (HCD)           : Got : CTRL+D" );
			// Used to break out of Wait so we can check ua_Counter in Stopping
		}

//		usbbase->usb_IExec->DebugPrintF( "\n" );
	}

//	usbbase->usb_IExec->DebugPrintF( "HCD Shutting down\n" );

	TASK_NAME_LEAVE();
}

// --
