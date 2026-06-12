
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

SEC_CODE void __Driver_Unlock( struct USBBase *usbbase, struct USB3_DriverNode *dn, STR file UNUSED )

#else

SEC_CODE void __Driver_Unlock( struct USBBase *usbbase, struct USB3_DriverNode *dn )

#endif

{
	TASK_NAME_ENTER( "__Driver_Unlock" );

	USBDEBUG( "__Driver_Unlock          : DN    %p : Locks %2ld (old) : (%s)", dn, ( dn )?dn->dn_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( DRIVER_VALID( dn ) == VSTAT_Okay )
	{
		if ( dn->dn_Locks > 0 )
		{
			dn->dn_Locks--;

//			USBDEBUG( "__Driver_Unlock          : DN    %p : New Lock Count : %ld (-1)", dn, dn->dn_Locks );
		}
		else
		{
			USBPANIC( "__Driver_Unlock          : DN    %p : Driver Node NOT Locked (%ld) :(%s)", dn, dn->dn_Locks, (file)?file:"<NULL>" );
		}
	}
	else
	{
		USBPANIC( "__Driver_Unlock          : DN    %p : Invalid Driver Node", dn );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
