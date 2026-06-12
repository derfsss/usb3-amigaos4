
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

// hmm should it only return when Counter is Zero
SEC_CODE S32 __ASync_Wait( struct USBBase *usbbase, struct USB2_ASync *ua )
{
S32 retval;

	TASK_NAME_ENTER( "__ASync_Wait" );

	USBDEBUG( "__ASync_Wait             : AS    %p", ua );

	retval = FALSE;

	#ifdef DO_PANIC

	if ( ! ua )
	{
		USBPANIC( "ASync_Wait : 1 : NULL Pointer" );
	}

	if ( ua->ua_StructID != ID_USB2_ASYNC )
	{
		USBDEBUG( "ASync_Wait : 2 : Invalid ID $%08lx", ua->ua_StructID );
		USBPANIC( "ASync_Wait : 2 : Invalid ID $%08lx", ua->ua_StructID );
	}

	#endif

	SEMAPHORE_OBTAIN( & ua->ua_Semaphore );

	if ( TASK_ALLOCSIGNAL( & ua->ua_Signal ))
	{
		ua->ua_Task = TASK_FIND();

		while( ua->ua_Counter )
		{
			SEMAPHORE_RELEASE( & ua->ua_Semaphore );

			while( TASK_WAIT( ua->ua_Signal.sig_Signal_Mask ) != ua->ua_Signal.sig_Signal_Mask );

			SEMAPHORE_OBTAIN( & ua->ua_Semaphore );
		}

		ua->ua_Task = NULL;
		TASK_FREESIGNAL( & ua->ua_Signal );
	}

	SEMAPHORE_RELEASE( & ua->ua_Semaphore );

	TASK_NAME_LEAVE();
	return( retval );
}

// --
