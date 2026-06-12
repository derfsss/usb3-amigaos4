
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Misc_ReleaseInterface( struct USBBase *usbbase, PTR ifc )
{
	TASK_NAME_ENTER( "__EXEC_ReleaseInterface" );

	if ( ifc )
	{
		usbbase->usb_IExec->DropInterface( ifc );
	}

	TASK_NAME_LEAVE();
}

// --
