
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

SEC_CODE enum LSTAT __Driver_Lock( struct USBBase *usbbase, struct USB2_DriverNode *dn, STR file UNUSED )

#else

SEC_CODE enum LSTAT __Driver_Lock( struct USBBase *usbbase, struct USB2_DriverNode *dn )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__Driver_Lock" );

	USBDEBUG( "__Driver_Lock            : DN    %p : Locks %2ld (old) : (%s)", dn, ( dn )?dn->dn_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( DRIVER_VALID( dn ) == VSTAT_Okay )
	{
		dn->dn_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__Driver_Lock            : DN    %p : New Lock Count : %ld (+1)", dn, dn->dn_Locks );
	}
	else
	{
		USBPANIC( "__Driver_Lock            : DN    %p : Lock failed : (%s)", dn, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
