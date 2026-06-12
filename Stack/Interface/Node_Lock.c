
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

SEC_CODE enum LSTAT __Interface_LockNode( struct USBBase *usbbase, struct USB2_InterfaceNode *in, STR file UNUSED )

#else

SEC_CODE enum LSTAT __Interface_LockNode( struct USBBase *usbbase, struct USB2_InterfaceNode *in )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__Interface_LockNode" );

	USBDEBUG( "__Interface_LockNode     : IN    %p : (%s)", in, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( INTERFACE_VALIDNODE( in ) == VSTAT_Okay )
	{
		in->in_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__Interface_LockNode     : IN    %p : New Lock Count : %ld (+1)", in, in->in_Locks );
	}
	else
	{
		USBPANIC( "__Interface_LockNode     : IN    %p :Lock failed", in );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
