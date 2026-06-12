
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

SEC_CODE void __MsgPort_Free( struct USBBase *usbbase, struct USB2_MsgPort *mp, STR file UNUSED )

#else

SEC_CODE void __MsgPort_Free( struct USBBase *usbbase, struct USB2_MsgPort *mp )

#endif

{
	TASK_NAME_ENTER( "__MsgPort_Free" );

	if (( mp == NULL )
	||	( mp->ump_StructID == 0 ) 
	||	( mp->ump_StructID == ID_USB2_FREED ))
	{
		goto bailout;
	}

	#ifdef DO_PANIC

	if ( mp->ump_StructID != ID_USB2_MP )
	{
		USBPANIC( "MsgPort_Free  :  2 : IN    use : (%s)", file );
	}

	#endif

	mp->ump_StructID = ID_USB2_FREED;

	// should proberly free any message in mp_MsgList

	TASK_FREESIGNAL( & mp->ump_Signal );

bailout:

	TASK_NAME_LEAVE();
}

// --
