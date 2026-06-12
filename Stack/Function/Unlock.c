
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

SEC_CODE void __Function_Unlock( struct USBBase *usbbase, struct RealFunctionNode *fn, STR file UNUSED )

#else

SEC_CODE void __Function_Unlock( struct USBBase *usbbase, struct RealFunctionNode *fn )

#endif

{
	TASK_NAME_ENTER( "__Function_Unlock" );

	USBDEBUG( "__Function_Unlock        : FN    %p : Locks %2ld (old) : (%s)", fn, fn->fkt_Locks, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( FUNCTION_VALID(fn) == VSTAT_Okay )
	{
		if ( fn->fkt_Locks > 0 )
		{
			fn->fkt_Locks--;

//			USBDEBUG( "__Function_Unlock        : FN    %p : New Lock Count : %ld (-1)", fn, fn->fkt_Locks );
		}
		else
		{
			USBPANIC( "__Function_Unlock        : FN    %p : Function NOT Locked (%ld)", fn, fn->fkt_Locks );
		}
	}
	else
	{
		USBERROR( "__Function_Unlock        : FN    %p : Invalid Function Node", fn );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --

