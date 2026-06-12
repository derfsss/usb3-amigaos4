
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

SEC_CODE enum LSTAT __EndPointRes_Lock( struct USBBase *usbbase, struct RealEndPointResource *epr, STR file UNUSED )

#else

SEC_CODE enum LSTAT __EndPointRes_Lock( struct USBBase *usbbase, struct RealEndPointResource *epr )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__EndPointRes_Lock" );

	USBDEBUG( "__EndPointRes_Lock       : EPR   %p : Locks %2ld (old) : (%s)", epr, (epr)?epr->epr_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( ENDPOINTRES_VALID(epr) == VSTAT_Okay )
	{
		epr->epr_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__EndPointRes_Lock       : EPR   %p : New Lock Count : %ld (+1)", epr, epr->epr_Locks );
	}
	else
	{
		USBPANIC( "__EndPointRes_Lock       : EPR   %p : Lock failed : (%s)", epr, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
