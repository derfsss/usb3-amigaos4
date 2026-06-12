
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Misc_CreateLibrary( struct USBBase *usbbase, PTR taglist )
{
PTR retval;

	if ( taglist )
	{
		retval = usbbase->usb_IExec->CreateLibrary( taglist );
	}
	else
	{
		retval = NULL;
	}

	return(	retval );
}

// --
