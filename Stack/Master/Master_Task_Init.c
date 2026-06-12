
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

SEC_CODE static S32 __myTask_Init( struct USBBase *usbbase, struct intern *in )
{
S32 retval;

	retval = FALSE;
	USBINFO( "__myTask_Init (Master)" );

	// --

	if ( ! __myTask_Def_Init( usbbase, in, FALSE ))
	{
		USBERROR( "__myTask_Init (Master) : 1 : Error" );
		goto bailout;
	}

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --
