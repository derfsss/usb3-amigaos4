
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

SEC_CODE void __ASync_Add( struct USBBase *usbbase, struct USB3_ASync *as )
{
//	USBDEBUG( "__ASync_Add              : AS    %p", as );
	TASK_NAME_ENTER( "__ASync_Add" );


	if ( ! as )
	{
		TASK_NAME_LEAVE();
		return;
	}

	if (( as->ua_StructID == 0 ) 
	||	( as->ua_StructID == ID_USB3_FREED ))
	{
		// And if this fails?? Painc??
		ASYNC_INIT( as, NULL );
	}

	#ifdef DO_PANIC

	if ( as->ua_StructID != ID_USB3_ASYNC )
	{
		USBPANIC( "__ASync_Add  :  2 : Invalid ID" );
	}

	#endif

	SEMAPHORE_OBTAIN( & as->ua_Semaphore );

	as->ua_Counter++;
//	USBDEBUG( "__ASync_Add              : AS    %p : New Count : %ld (+1)", as, as->ua_Counter );

	SEMAPHORE_RELEASE( & as->ua_Semaphore );

	TASK_NAME_LEAVE();
}

// --
