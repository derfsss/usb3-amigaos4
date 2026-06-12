
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Misc_CloseFile( struct USBBase *usbbase, S32 handle )
{
	TASK_NAME_ENTER( "__DOS_CloseFile" );

	if ( handle )
	{
		usbbase->usb_IDOS->Close( handle );
	}

	TASK_NAME_LEAVE();
}

// --
