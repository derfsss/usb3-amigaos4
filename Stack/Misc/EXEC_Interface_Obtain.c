
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Misc_ObtainInterface( struct USBBase *usbbase, PTR base, STR name, U32 ver )
{
PTR ifc;

	TASK_NAME_ENTER( "__EXEC_ObtainInterface" );

	if ( base )
	{
		ifc = usbbase->usb_IExec->GetInterface( base, name, ver, NULL );
	}
	else
	{
		ifc = NULL;
	}

	TASK_NAME_LEAVE();

	return( ifc );
}

// --
