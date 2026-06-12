
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

SEC_CODE void __ASync_Sub( struct USBBase *usbbase, struct USB2_ASync *as )
{
//	USBDEBUG( "__ASync_Sub              : AS    %p", as );

	TASK_NAME_ENTER( "__ASync_Sub" );
	TASK_NAME_LEAVE();

	if ( ! as )
	{
		TASK_NAME_LEAVE();
		return;
	}

	#ifdef DO_PANIC

	if ( as->ua_StructID != ID_USB2_ASYNC )
	{
		USBPANIC( "ASync_Sub : 3 : Invalid ID : AS    %p", as );
	}

	#endif

	SEMAPHORE_OBTAIN( & as->ua_Semaphore );

	if ( as->ua_Counter > 0 )
	{
		as->ua_Counter--;

		if ( as->ua_Parent )
		{
			USBDEBUG( "__ASync_Sub              : Signal CTRL+D" );
			TASK_SIGNAL( as->ua_Parent, SIGBREAKF_CTRL_D );
		}

//		USBDEBUG( "__ASync_Sub              : AS    %p : New Count : %ld (-1)", as, as->ua_Counter );
	}

	if (( as->ua_Counter <= 0 )
	&&	( as->ua_Task ))
	{
		// Signal ASync_Wait()
		USBDEBUG( "__ASync_Sub              : Signal : AS    %p", as );
		TASK_SIGNAL( as->ua_Task, as->ua_Signal.sig_Signal_Mask );
	}

	SEMAPHORE_RELEASE( & as->ua_Semaphore );

	TASK_NAME_LEAVE();
}

// --
