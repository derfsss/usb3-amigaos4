
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

SEC_CODE void __EndPoint_Unlock( struct USBBase *usbbase, struct USB2_EndPointNode *ep, STR file UNUSED )

#else

SEC_CODE void __EndPoint_Unlock( struct USBBase *usbbase, struct USB2_EndPointNode *ep )

#endif

{
	TASK_NAME_ENTER( "__EndPoint_Unlock" );

	USBDEBUG( "__EndPoint_Unlock        : EP    %p : Locks %2ld (old) : (%s)", ep, (ep)?ep->ep_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( ENDPOINT_VALID(ep) == VSTAT_Okay )
	{
		if ( ep->ep_Locks > 0 )
		{
			ep->ep_Locks--;

//			USBDEBUG( "__EndPoint_Unlock        : EP    %p : New Lock Count : %ld (-1)", ep, ep->ep_Locks );
		}
		else
		{
			USBPANIC( "__EndPoint_Unlock        : EP    %p : EndPoint NOT Locked (%ld)", ep, ep->ep_Locks );
		}
	}
	else
	{
		USBPANIC( "__EndPoint_Unlock        : EP    %p : Invalid EndPoint Node", ep );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
