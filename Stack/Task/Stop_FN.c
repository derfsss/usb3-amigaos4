
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

SEC_CODE void __Task_Stop_FN( struct USBBase *usbbase, struct RealFunctionNode *fn, STR file UNUSED )

#else

SEC_CODE void __Task_Stop_FN( struct USBBase *usbbase, struct RealFunctionNode *fn )

#endif

{
struct USB2_DriverNode *dn;

	USBINFO( "__Task_Stop_FN           :  1 : FN %p : (%s)", fn, (file)?file:"<NULL>" );

	if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
	{
		SEMAPHORE_OBTAIN( & usbbase->usb_Bind_Semaphore );

		dn = usbbase->usb_Bind_Header.uh_Head;

		while( dn )
		{
			USBINFO( "__Task_Stop_FN           :  2 : DN %p", dn );

			if ( dn->dn_Function == fn )
			{
				USBINFO( "__Task_Stop_FN           :  3 : DN %p : Calling TASK_STOP_DN()", dn );

				TASK_STOP_DN( dn );
			}

			dn = Node_Next( dn );
		}

		SEMAPHORE_RELEASE( & usbbase->usb_Bind_Semaphore );

		FUNCTION_UNLOCK( fn );
	}
	else
	{
		USBINFO( "__Task_Stop_FN           :  4 : Invalid FN %p", fn );
	}

	USBINFO( "__Task_Stop_FN           : 99 : FN %p", fn );
	return;
}

// --
