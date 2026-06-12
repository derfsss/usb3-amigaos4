
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

SEC_CODE struct USB2_TaskMsg *__TaskMsg_Alloc( struct USBBase *usbbase, STR file UNUSED )

#else

SEC_CODE struct USB2_TaskMsg *__TaskMsg_Alloc( struct USBBase *usbbase )

#endif

{
struct USB2_TaskMsg *tm;
S32 error;

	tm = NULL;

	error = TRUE;

//	USBDEBUG( "__TaskMsg_Alloc          : 11 :" );

	tm = MEMORY_ALLOC( MEMID_USBTaskMsg, TRUE );

	if ( ! tm )
	{
		USBERROR( "__TaskMsg_Alloc          : 12 : Error allocating memory" );
		goto bailout;
	}

	tm->tm_StructID = ID_USB2_TMSG;

	// --

	error = FALSE;

bailout:

	if ((tm) && ( error ))
	{
		USBDEBUG( "__TaskMsg_Alloc          : 13 : Free PMsg %p", tm );
		TASKMSG_FREE(tm);
		tm = NULL;
	}

	return( tm );
}

// --
