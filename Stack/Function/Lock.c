
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

SEC_CODE enum LSTAT __Function_Lock( struct USBBase *usbbase, struct RealFunctionNode *fn, STR file UNUSED )

#else

SEC_CODE enum LSTAT __Function_Lock( struct USBBase *usbbase, struct RealFunctionNode *fn )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__Function_Lock" );

	USBDEBUG( "__Function_Lock          : FN    %p : Locks %2ld (old) : (%s)", fn, (fn)?fn->fkt_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( FUNCTION_VALID(fn) == VSTAT_Okay )
	{
		fn->fkt_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__Function_Lock          : FN    %p : New Lock Count : %ld (+1)", fn, fn->fkt_Locks );
	}
	else
	{
		USBPANIC( "__Function_Lock          : FN    %p : Lock failed : (%s)", fn, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
