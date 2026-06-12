
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

SEC_CODE enum LSTAT __Config_Lock( struct USBBase *usbbase, struct USB2_ConfigNode *cn, STR file UNUSED )

#else

SEC_CODE enum LSTAT __Config_Lock( struct USBBase *usbbase, struct USB2_ConfigNode *cn )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__Config_Lock" );

//	USBDEBUG( "__Config_Lock            : CN    %p : Locks %2ld (old) : (%s)", cn, (cn)?cn->cfg_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( CONFIG_VALID(cn) == VSTAT_Okay )
	{
		cn->cfg_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__Config_Lock            : CN    %p : New Lock Count : %ld (+1)", cn, cn->cfg_Locks );
	}
	else
	{
		USBPANIC( "__Config_Lock            : CN    %p : Lock failed : (%s)", cn, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
