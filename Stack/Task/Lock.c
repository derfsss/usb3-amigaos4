
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

SEC_CODE enum LSTAT __Task_Lock( struct USBBase *usbbase, struct USB3_TaskNode *tn, STR file UNUSED )

#else

SEC_CODE enum LSTAT __Task_Lock( struct USBBase *usbbase, struct USB3_TaskNode *tn )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__Task_Lock" );

	USBDEBUG( "__Task_Lock              : TN %p : Locks %ld (old) : (%s)", tn, (tn)?tn->tn_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( TASK_VALID(tn) == VSTAT_Okay )
	{
		tn->tn_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__Task_Lock              : TN %p : New Lock Count : %ld (+1)", tn, tn->tn_Locks );
	}
	else
	{
		USBPANIC( "__Task_Lock              : TN %p : Lock failed : (%s)", tn, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
