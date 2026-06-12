
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Misc_OpenLibrary( struct USBBase *usbbase, STR name, U32 ver )
{
PTR base;

	TASK_NAME_ENTER( "__EXEC_OpenLibrary" );

	if ( name )
	{
		base = usbbase->usb_IExec->OpenLibrary( name, ver );
	}
	else
	{
		base = NULL;
	}

	TASK_NAME_LEAVE();

	return( base );
}

// --
