
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

S32 __myMaster_StartPromote( struct USBBase *usbbase );

// --

SEC_CODE static S32 __myTask_Main( struct USBBase *usbbase, struct intern *in )
{
S32 retval;
U32 mask;
U32 wait;

	USBDEBUG( "__myTask_Main (Master)   : Running" );

	retval = MAIN_RETURN_UNSET;

	while( TRUE )
	{
		/*
		** Check if we should exit
		*/

		#if 0
		USBDEBUG( 
			"__myTask_Main (Master)\n"
			"  :: Stopping ...... : %ld\n"
			"  :: TM    Nodes ...... : %ld (TaskMsg's)\n"
			"  :: TM    First ...... : %p\n"
			"  :: TN    Nodes ...... : %ld (Task's)\n"
			"  :: TN    First ...... : %p\n"
			"  :: HCD ASync Adr . : %p\n"
			"  :: HCD ASync Cnt . : %ld\n"
			"  ::\n", 
				in->Stopping, 
				in->Shutdown_TM_List.uh_Nodes, 
				in->Shutdown_TM_List.uh_Head,
				usbbase->usb_Task_Header.uh_Nodes,
				usbbase->usb_Task_Header.uh_Head,
				& usbbase->usb_HCDASync,
				usbbase->usb_HCDASync.ua_Counter
		);
		#endif

		#if 0
		usbbase->usb_IExec->DebugPrintF(
			"__myTask_Main (Master)\n"
			"  :: Stopping ...... : %ld\n"
			"  :: TM    Nodes ...... : %ld (TaskMsg's)\n"
			"  :: TM    First ...... : %p\n"
			"  :: TN    Nodes ...... : %ld (Task's)\n"
			"  :: TN    First ...... : %p\n"
			"  :: HCD ASync Adr . : %p\n"
			"  :: HCD ASync Cnt . : %ld\n"
			"  ::\n", 
				in->Stopping, 
				in->Shutdown_TM_List.uh_Nodes, 
				in->Shutdown_TM_List.uh_Head,
				usbbase->usb_Task_Header.uh_Nodes,
				usbbase->usb_Task_Header.uh_Head,
				& usbbase->usb_HCDASync,
				usbbase->usb_HCDASync.ua_Counter
		);
		#endif

		if (( in->Stopping )
		&&	( in->Shutdown_TM_List.uh_Nodes <= 0 )
		&&	( usbbase->usb_HCDASync.ua_Counter <= 0 ))
		{
			USBDEBUG( "__myTask_Main (Master)   : Exit" );
			retval = MAIN_RETURN_EXIT;
			break;
		}

		/*
		** Build Wait Mask
		*/

		// Master Exit signal
		wait  = SIGBREAKF_CTRL_C;

		// Master Refresh signal
		wait |= SIGBREAKF_CTRL_D;

		// Fullboot signal
		wait |= SIGBREAKF_CTRL_E;

		// a Subtask stopped
		wait |= in->Task_Shutdown_Signal.sig_Signal_Mask;

		// a Subtask dident stop in time .. timeout, crashed?
		wait |= in->Task_Shutdown_MsgPort.ump_Signal.sig_Signal_Mask;

		// External User Command
		wait |= in->Cmd_MsgPort.ump_Signal.sig_Signal_Mask;

		// Tick Timer
		wait |= in->Tick_MsgPort.ump_Signal.sig_Signal_Mask;

		// Notify Reply
		wait |= in->Notify_ReplyMsgPort.ump_Signal.sig_Signal_Mask;

		mask  = TASK_WAIT( wait );


		/*
		** Check Flags
		*/

		// Master Exit signal
		if ( mask & SIGBREAKF_CTRL_C )
		{
			if ( usbbase->usb_Master_Task.tn_Parent )
			{
				USBDEBUG( "__myTask_Main (Master)   : Got : CTRL+C : Exit Signal" );
				in->Stopping = TRUE;
			}
			else
			{
				USBDEBUG( "__myTask_Main (Master)   : Got : CTRL+C : Someone tryed to stop us, ignoring" );
			}
		}

		if ( mask & SIGBREAKF_CTRL_D )
		{
			// Just a refresh, so we can break the Wait()
			USBDEBUG( "__myTask_Main (Master)   : Got : CTRL+D" );
			myFree_DNs( usbbase );
			myFree_TNs( usbbase );
		}

		if ( mask & SIGBREAKF_CTRL_E )
		{
			// Fullboot signal, DOS is now around
//			usbbase->usb_IExec->DebugPrintF( "\nFullboot Signal\n\n" );

			if ( ! in->Stopping )
			{
				if ( __myMaster_StartPromote( usbbase ))
				{
					USBDEBUG( "__myTask_Main : Promote exit\n" );
					retval = MAIN_RETURN_PROMOTE;
					break; // exit right away
				}
			}
		}

		if ( mask & in->Tick_MsgPort.ump_Signal.sig_Signal_Mask )
		{
//			USBDEBUG( "__myTask_Main (Master)   : Got : Tick Timer" );
			if ( MSGPORT_GETMSG( & in->Tick_MsgPort ))
			{
				myFree_DNs( usbbase );
				myFree_TNs( usbbase );
				in->Tick_TimeRequest.Time.Seconds = 2;
				in->Tick_TimeRequest.Time.Microseconds = 0;
				IO_SEND( & in->Tick_TimeRequest );
			}
		}

		// a Subtask ended
		if ( mask & in->Task_Shutdown_Signal.sig_Signal_Mask )
		{
			USBDEBUG( "__myTask_Main (Master)   : Got : Task_Shutdown_Signal" );
			myHandle_Task_Ended( usbbase, in );
		}

		// a Subtask dident stop in time .. timeout, crashed?
		if ( mask & in->Task_Shutdown_MsgPort.ump_Signal.sig_Signal_Mask )
		{
			USBDEBUG( "__myTask_Main (Master)   : Got : Task_Shutdown_MsgPort" );
			myHandle_Task_Timeout( usbbase, in );
		}

		// External User Command
		if ( mask & in->Cmd_MsgPort.ump_Signal.sig_Signal_Mask )
		{
			USBDEBUG( "__myTask_Main (Master)   : Got : Cmd_MsgPort" );
			myHandle_Cmds( usbbase, in );
		}

		// Notify Reply
		if ( mask & in->Notify_ReplyMsgPort.ump_Signal.sig_Signal_Mask )
		{
			USBDEBUG( "__myTask_Main (Master)   : Got : Notify_Reply" );

			while( TRUE )
			{
				PTR msg = MSGPORT_GETMSG( & in->Notify_ReplyMsgPort );

				if ( ! msg )
				{
					break;
				}

				MEMORY_FREE( MEMID_NotifyMessage, msg, 0 );
			}
		}
	}

	return( retval );
}

// --
