
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

SEC_CODE struct RealEndPointResource *__EndPointRes_Alloc( struct USBBase *usbbase, STR file UNUSED )

#else

SEC_CODE struct RealEndPointResource *__EndPointRes_Alloc( struct USBBase *usbbase )

#endif

{
struct RealEndPointResource *epr;
S32 error;

	error = TRUE;

	USBDEBUG( "__EndPointRes_Alloc      : (%s)", (file)?file:"<NULL>" );

	epr = MEMORY_ALLOC( MEMID_USBEndPointResource, TRUE );

	if ( ! epr )
	{
		USBERROR( "__EndPointRes_Alloc : Error allocating memory" );
		goto bailout;
	}

	epr->epr_StructID = ID_USB3_EPR;

	// --

	error = FALSE;

bailout:

	if ((epr) && ( error ))
	{
		ENDPOINTRES_FREE(epr);
		epr = NULL;
	}

	USBDEBUG( "__EndPointRes_Alloc      : EPR   %p : (new)", epr );

	return( epr );
}

// --
