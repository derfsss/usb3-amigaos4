
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

SEC_CODE void __TaskMsg_Unlock( struct USBBase *usbbase, struct USB3_TaskMsg *tm, STR file UNUSED )

#else

SEC_CODE void __TaskMsg_Unlock( struct USBBase *usbbase, struct USB3_TaskMsg *tm )

#endif

{
	TASK_NAME_ENTER( "__TaskMsg_Unlock" );

	USBDEBUG( "__TaskMsg_Unlock         : TM %p : Locks %ld (old) : (%s)", tm, (tm)?tm->tm_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( TASKMSG_VALID(tm) == VSTAT_Okay )
	{
		if ( tm->tm_Locks > 0 )
		{
			tm->tm_Locks--;

//			USBDEBUG( "__TaskMsg_Unlock         : TM %p : New Lock Count : %ld (-1)", tm, tm->tm_Locks );
		}
		else
		{
			USBPANIC( "__TaskMsg_Unlock         : TM %p : TaskMsg NOT Locked (%ld)", tm, tm->tm_Locks );
		}
	}
	else
	{
		USBERROR( "__TaskMsg_Unlock         : TM %p : Invalid TaskMsg", tm );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
