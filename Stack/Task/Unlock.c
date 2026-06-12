
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

SEC_CODE void __Task_Unlock( struct USBBase *usbbase, struct USB2_TaskNode *tn, STR file UNUSED )

#else

SEC_CODE void __Task_Unlock( struct USBBase *usbbase, struct USB2_TaskNode *tn )

#endif

{
	TASK_NAME_ENTER( "__Task_Unlock" );

	USBDEBUG( "__Task_Unlock            : TN %p : Locks %ld (old) : (%s)", tn, (tn)?tn->tn_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( TASK_VALID(tn) == VSTAT_Okay )
	{
		if ( tn->tn_Locks > 0 )
		{
			tn->tn_Locks--;

//			USBDEBUG( "__Task_Unlock            : TN %p : New Lock Count : %ld (-1)", tn, tn->tn_Locks );
		}
		else
		{
			USBPANIC( "__Task_Unlock            : TN %p : Driver Node NOT Locked (%ld)", tn, tn->tn_Locks );
		}
	}
	else
	{
		USBERROR( "__Task_Unlock            : TN %p : Invalid Driver Node", tn );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}


// --
