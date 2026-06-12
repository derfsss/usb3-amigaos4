
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Misc_OpenFile( struct USBBase *usbbase, STR filename, U32 filemode )
{
S32 handle;

	TASK_NAME_ENTER( "__DOS_OpenFile" );

	if ( filename )
	{
		handle = usbbase->usb_IDOS->Open( filename, filemode );
	}
	else
	{
		handle = 0;
	}

	TASK_NAME_LEAVE();

	return( handle );
}

// --
