
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

SEC_CODE void __MsgPort_ReplyMsg( struct USBBase *usbbase, PTR msg, STR file UNUSED )

#else

SEC_CODE void __MsgPort_ReplyMsg( struct USBBase *usbbase, PTR msg )

#endif

{
struct ExecIFace *IExec;

	TASK_NAME_ENTER( "__MsgPort_ReplyMsg" );

	#ifdef DO_PANIC

	if (( ! usbbase ) || ( ! msg ))
	{
		USBPANIC( "MsgPort_ReplyMsg : 3 : NULL Pointer : (%s)", file );
	}

	#endif

	IExec = usbbase->usb_IExec;
	IExec->ReplyMsg( msg );

	TASK_NAME_LEAVE();
}

// --
