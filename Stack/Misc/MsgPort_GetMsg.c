
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

SEC_CODE PTR __MsgPort_GetMsg( struct USBBase *usbbase, struct USB3_MsgPort *mp, STR file UNUSED )

#else

SEC_CODE PTR __MsgPort_GetMsg( struct USBBase *usbbase, struct USB3_MsgPort *mp )

#endif

{
struct ExecIFace *IExec;
PTR msg;

	TASK_NAME_ENTER( "__MsgPort_GetMsg" );

	#ifdef DO_PANIC

	if (( ! usbbase ) || ( ! mp ))
	{
		USBPANIC( "MsgPort_GetMsg : 1 : NULL Pointer : (%s)", file );
	}

	if ( mp->ump_StructID != ID_USB3_MP )
	{
		USBPANIC( "MsgPort_GetMsg : 2 : Invalid ID : (%s)", file );
	}

	#endif

	IExec = usbbase->usb_IExec;
	msg = IExec->GetMsg( & mp->ump_MsgPort );

	TASK_NAME_LEAVE();

	return( msg );
}

// --
