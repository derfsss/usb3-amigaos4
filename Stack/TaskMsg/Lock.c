
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

SEC_CODE enum LSTAT __TaskMsg_Lock( struct USBBase *usbbase, struct USB3_TaskMsg *tm, STR file UNUSED )

#else

SEC_CODE enum LSTAT __TaskMsg_Lock( struct USBBase *usbbase, struct USB3_TaskMsg *tm )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__TaskMsg_Lock" );

	USBDEBUG( "__TaskMsg_Lock           : TM %p : Locks %ld (old) : (%s)", tm, (tm)?tm->tm_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( TASKMSG_VALID(tm) == VSTAT_Okay )
	{
		tm->tm_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__TaskMsg_Lock           : TM %p : New Lock Count : %ld (+1)", tm, tm->tm_Locks );
	}
	else
	{
		USBPANIC( "__TaskMsg_Lock           : TM %p : Lock failed : (%s)", tm, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
