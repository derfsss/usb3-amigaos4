
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

SEC_CODE void __MsgPort_PutMsg( struct USBBase *usbbase, struct USB3_MsgPort *mp, PTR msg, STR file UNUSED )

#else

SEC_CODE void __MsgPort_PutMsg( struct USBBase *usbbase, struct USB3_MsgPort *mp, PTR msg )

#endif

{
struct ExecIFace *IExec;

	TASK_NAME_ENTER( "__MsgPort_PutMsg" );

	#ifdef DO_PANIC

	if ( ! mp )
	{
		USBPANIC( "MsgPort_PutMsg : 1 : NULL Pointer : (%s)", file );
	}

	if ( mp->ump_StructID != ID_USB3_MP )
	{
		USBPANIC( "MsgPort_PutMsg : 2 : IN    use : (%s)", file );
	}

	if ( ! msg )
	{
		USBPANIC( "MsgPort_PutMsg : 3 : NULL Pointer : (%s)", file );
	}

	#endif

	IExec = usbbase->usb_IExec;
	IExec->PutMsg( & mp->ump_MsgPort, msg );

	TASK_NAME_LEAVE();
}

// --
