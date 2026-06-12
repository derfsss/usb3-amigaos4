
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

SEC_CODE void __Task_Stop_DN( struct USBBase *usbbase, struct USB3_DriverNode *dn, STR file UNUSED )

#else

SEC_CODE void __Task_Stop_DN( struct USBBase *usbbase, struct USB3_DriverNode *dn )

#endif

{
struct USB3_TaskNode *tn;

	USBDEBUG( "__Task_Stop_DN           :  1 : DN %p : (%s)", dn, (file)?file:"<NULL>" );

	if ( DRIVER_LOCK( dn ) == LSTAT_Okay )
	{
		// User/Stack want to stop driver
		tn = dn->dn_Task;

		if ( TASK_LOCK( tn ) == LSTAT_Okay )
		{
			USBDEBUG( "__Task_Stop_DN           :  2 : TN %p : Calling TASK_STOP_TN()", tn );

			TASK_STOP_TN( tn );

			TASK_UNLOCK( tn );
		}
		else
		{
			USBDEBUG( "__Task_Stop_DN           :  3 : Invalid TN %p : (%s)", tn, (file)?file:"<NULL>" );
		}
				
		DRIVER_UNLOCK( dn );
	}
	else
	{
		USBDEBUG( "__Task_Stop_DN           :  4 : Invalid DN %p : (%s)", dn, (file)?file:"<NULL>" );
	}

	USBDEBUG( "__Task_Stop_DN           : 99 : DN %p", dn );
}

// --
