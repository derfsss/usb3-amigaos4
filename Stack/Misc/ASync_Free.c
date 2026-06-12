
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

/*
	Yes .. the point of ASync.. 
	
	is USB Device shutdown in async mode, 

	so HUB and HCD can signal all its
	devices to quit and wait for it
	to finish.
*/

// --

SEC_CODE void __ASync_Free( struct USBBase *usbbase, struct USB3_ASync *ua )
{
	USBDEBUG( "__ASync_Free             : AS    %p", ua );

	TASK_NAME_ENTER( "__ASync_Free" );

	if (( ua == NULL )
	||	( ua->ua_StructID == 0 ) 
	||	( ua->ua_StructID == ID_USB3_FREED ))
	{
		TASK_NAME_LEAVE();
		return;
	}

	#ifdef DO_PANIC

	if ( ua->ua_StructID != ID_USB3_ASYNC )
	{
		USBPANIC( "ASync_Free  :  2 : Invalid ID" );
	}

	#endif

	SEMAPHORE_OBTAIN( & ua->ua_Semaphore );

	ua->ua_StructID = ID_USB3_FREED;
	ua->ua_Parent = NULL;

	TASK_FREESIGNAL( & ua->ua_Signal );
	SEMAPHORE_FREE( & ua->ua_Semaphore );

	TASK_NAME_LEAVE();
}

// --
