
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

SEC_CODE S32 __ASync_Init( struct USBBase *usbbase, struct USB3_ASync *ua, struct Task *owner )
{
S32 retval;

	TASK_NAME_ENTER( "__ASync_Init" );

	retval = FALSE;

	USBDEBUG( "__ASync_Init             : AS    %p", ua );

	#ifdef DO_PANIC

	if ( ! ua )
	{
		USBPANIC( "ASync_Init : 1 : NULL Pointer" );
	}

	if (( ua->ua_StructID != 0 ) && ( ua->ua_StructID != ID_USB3_FREED ))
	{
		USBDEBUG( "__ASync_Init : id $%08lx", ua->ua_StructID );
		USBPANIC( "ASync_Init : 2 : IN    use" );
	}

	#endif

	MEM_SET( ua, 0, sizeof( struct USB3_ASync ));

	// Ohh what task should the Signal belong to??
	// maybe only ASync_Wait() should create it
//	if ( ! TASK_ALLOCSIGNAL( & ua->ua_Signal ))
//	{
//		goto bailout;
//	}

	SEMAPHORE_INIT( & ua->ua_Semaphore );

	ua->ua_StructID = ID_USB3_ASYNC;
//	ua->ua_Counter = 0;
//	ua->ua_Signal = NULL;
//	ua->ua_Task = NULL;
	ua->ua_Parent = owner;

	retval = TRUE;

// bailout:

	TASK_NAME_LEAVE();
	return( retval ); 
}

// --
