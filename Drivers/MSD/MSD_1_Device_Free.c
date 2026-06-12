
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --

SEC_CODE void MSD_Device_Free( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED )
{
	TASK_NAME_ENTER( "MSD_Device_Free" );
	USBDEBUG( "MSD : MSD_Device_Free" );

	// --


	// --

	TASK_NAME_LEAVE();
}

// --
