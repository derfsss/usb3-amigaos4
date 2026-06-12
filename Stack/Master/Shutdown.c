
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Master_Shutdown( struct USBBase *usbbase )
{
U32 mask;

	USBDEBUG( "__Master_Shutdown        : Task %p", usbbase->usb_Master_Task.tn_TaskAdr );

	if ( usbbase->usb_Master_Task.tn_TaskAdr )
	{
		USBDEBUG( "__Master_Shutdown        : 11 :" );

		usbbase->usb_Master_Task.tn_Parent = TASK_FIND();
		usbbase->usb_Master_Task.tn_Mask = SIGBREAKF_CTRL_F;

		TASK_SIGNAL( usbbase->usb_Master_Task.tn_TaskAdr, SIGBREAKF_CTRL_C );

		USBDEBUG( "__Master_Shutdown        : 22 :" );

		while( TRUE )
		{
			mask = TASK_WAIT( SIGBREAKF_CTRL_F );

			if ( mask & SIGBREAKF_CTRL_F )
			{
				break;
			}
		}
	}

	USBDEBUG( "__Master_Shutdown        : 33 :" );
}

// --
