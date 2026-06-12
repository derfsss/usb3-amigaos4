
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Misc_SetProcWindow( struct USBBase *usbbase, S32 val )
{
struct DOSIFace *IDOS;
S32 retval;

	TASK_NAME_ENTER( "__DOS_SetProcWindow" );

	IDOS = usbbase->usb_IDOS;

	if ( IDOS )
	{
		retval = (S32) IDOS->SetProcWindow( (PTR) val );
	}
	else
	{
		retval = 0;
	}

	TASK_NAME_LEAVE();

	return( retval );
}

// --
