
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if 0

struct CmdHeader
{
	struct Message		cm_Message;
	enum USB3_Cmd		cm_Command;
	struct USBBase *	cm_usbbase;
	U32					cm_Result;
	struct Task *		cm_Task;
	U32					cm_Mask;
};

#endif

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE U32 __Misc_DoCommand( struct USBBase *usbbase, struct CmdHeader *ch, STR file UNUSED )

#else

SEC_CODE U32 __Misc_DoCommand( struct USBBase *usbbase, struct CmdHeader *ch )

#endif

{
struct USB3_Signal sig;
U32 retval;
U32 mask;

	TASK_NAME_ENTER( "__Misc_DoCommand" );

	// --

	#ifdef DO_PANIC

	if ( TASK_FIND() == usbbase->usb_Master_Task.tn_TaskAdr )
	{
		// This will dead lock, as there will never be a responce
		USBPANIC( "__Misc_DoCommand         : Do NOT run from Master Task : (%s)", file );
	}

	if ( ! usbbase->usb_Master_MsgPort )
	{
		USBPANIC( "__Misc_DoCommand         : Master MsgPort not setup : (%s)", file );
	}

	#endif

	// --

	if ( ! ch )
	{
		retval = FALSE;
		// panic or handle NULL Ptr
		USBPANIC( "__Misc_DoCommand         : NULL Pointer : (%s)", file );
		goto bailout;
	}

	// --

	// Alloc Signal
	TASK_ALLOCSIGNAL( & sig );
	mask = ( sig.sig_Signal_Nr < 0 ) ? SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F : sig.sig_Signal_Mask ;

	ch->cm_usbbase = usbbase;
	ch->cm_Result = FALSE;
	ch->cm_Task = TASK_FIND();
	ch->cm_Mask = mask;

	// Clear Signal
	// PutMsg invokes a task switch, so clear first
	// do I need to clear signal here?? well why not
	TASK_SETSIGNAL( 0, mask );

	USBDEBUG( "__Misc_DoCommand         : CH    %p : Enter", ch );
	MSGPORT_PUTMSG( usbbase->usb_Master_MsgPort, ch );
	while( TASK_WAIT( mask ) != mask );
	USBDEBUG( "__Misc_DoCommand         : CH    %p : Leave", ch );

	retval = ch->cm_Result;

	TASK_FREESIGNAL( & sig );

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
