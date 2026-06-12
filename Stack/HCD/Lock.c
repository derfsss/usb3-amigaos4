
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

SEC_CODE enum LSTAT __HCD_Lock( struct USBBase *usbbase, struct USB3_HCDNode *hn, STR file UNUSED )

#else

SEC_CODE enum LSTAT __HCD_Lock( struct USBBase *usbbase, struct USB3_HCDNode *hn )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__HCD_Lock" );

	USBDEBUG( "__HCD_Lock               : HN    %p : Locks %2ld (old) : (%s)", hn, ( hn )?hn->hn_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( HCD_VALID( hn ) == VSTAT_Okay )
	{
		hn->hn_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__HCD_Lock               : HN    %p : New Lock Count : %ld (+1)", hn, hn->hn_Locks );
	}
	else
	{
		USBPANIC( "__HCD_Lock               : HN    %p : Lock failed : (%s)", hn, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
