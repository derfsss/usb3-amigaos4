
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

SEC_CODE void __Interface_UnlockNode( struct USBBase *usbbase, struct USB2_InterfaceNode *in, STR file UNUSED )

#else

SEC_CODE void __Interface_UnlockNode( struct USBBase *usbbase, struct USB2_InterfaceNode *in )

#endif

{
	TASK_NAME_ENTER( "__Interface_UnlockNode" );

	USBDEBUG( "__Interface_UnlockNode   : IN    %p : (%s)", in, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( INTERFACE_VALIDNODE( in ) == VSTAT_Okay )
	{
		if ( in->in_Locks > 0 )
		{
			in->in_Locks--;

//			USBDEBUG( "__Interface_UnlockNode   : IN    %p : New Lock Count : %ld (-1)", in, in->in_Locks );
		}
		else
		{
			USBPANIC( "__Interface_UnlockNode   : IN    %p : Interface Node NOT Locked (%ld)", in, in->in_Locks );
		}
	}
	else
	{
		USBERROR( "__Interface_UnlockNode   : IN    %p : Invalid Interface Node", in );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
