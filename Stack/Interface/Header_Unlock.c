
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

SEC_CODE void __Interface_UnlockHeader( struct USBBase *usbbase, struct USB3_InterfaceHeader *ih, STR file UNUSED )

#else

SEC_CODE void __Interface_UnlockHeader( struct USBBase *usbbase, struct USB3_InterfaceHeader *ih )

#endif

{
	TASK_NAME_ENTER( "__Interface_UnlockHeader" );

	USBDEBUG( "__Interface_UnlockHeader : IH    %p : (%s)", ih, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( INTERFACE_VALIDHEADER(ih) == VSTAT_Okay )
	{
		if ( ih->ih_Locks > 0 )
		{
			ih->ih_Locks--;

//			USBDEBUG( "__Interface_UnlockHeader : IH    %p : New Lock Count : %ld (-1)", ih, ih->ih_Locks );
		}
		else
		{
			USBPANIC( "__Interface_UnlockHeader : IH    %p : Interface Header NOT Locked (%ld)", ih, ih->ih_Locks );
		}
	}
	else
	{
		USBERROR( "__Interface_UnlockHeader : IH    %p : Invalid Interface Header", ih );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
