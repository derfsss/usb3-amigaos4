
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

SEC_CODE void __Interface_UnlockGroup( struct USBBase *usbbase, struct USB3_InterfaceGroup *ig, STR file UNUSED )

#else

SEC_CODE void __Interface_UnlockGroup( struct USBBase *usbbase, struct USB3_InterfaceGroup *ig )

#endif

{
	TASK_NAME_ENTER( "__Interface_UnlockGroup" );

	USBDEBUG( "__Interface_UnlockGroup  : IG    %p : (%s)", ig, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( INTERFACE_VALIDGROUP(ig) == VSTAT_Okay )
	{
		if ( ig->ig_Locks > 0 )
		{
			ig->ig_Locks--;

//			USBDEBUG( "__Interface_UnlockGroup  : IG    %p : New Lock Count : %ld (-1)", ig, ig->ig_Locks );
		}
		else
		{
			USBPANIC( "__Interface_UnlockGroup  : IG    %p : Interface Group NOT Locked (%ld)", ig, ig->ig_Locks );
		}
	}
	else
	{
		USBERROR( "__Interface_UnlockGroup  : IG    %p : Invalid Interface Group", ig );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
