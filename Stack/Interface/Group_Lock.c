
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

SEC_CODE enum LSTAT __Interface_LockGroup( struct USBBase *usbbase, struct USB3_InterfaceGroup *ig, STR file UNUSED )

#else

SEC_CODE enum LSTAT __Interface_LockGroup( struct USBBase *usbbase, struct USB3_InterfaceGroup *ig )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__Interface_LockGroup" );

	USBDEBUG( "__Interface_LockGroup    : IG    %p : (%s)", ig, file);

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( INTERFACE_VALIDGROUP(ig) == VSTAT_Okay )
	{
		ig->ig_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__Interface_LockGroup    : IG    %p : New Lock Count : %ld (+1)", ig, ig->ig_Locks );
	}
	else
	{
		USBPANIC( "__Interface_LockGroup    : IG    %p : Lock failed : (%s)", ig, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
