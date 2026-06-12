
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Misc_SetCurrentDir( struct USBBase *usbbase, S32 new )
{
struct DOSIFace *IDOS;
S32 old;

	TASK_NAME_ENTER( "__DOS_SetCurrentDir" );

	IDOS = usbbase->usb_IDOS;

	if ( IDOS )
	{
		old = IDOS->SetCurrentDir( new );
	}
	else
	{
		old = 0;
	}

	TASK_NAME_LEAVE();

	return( old );
}

// --
