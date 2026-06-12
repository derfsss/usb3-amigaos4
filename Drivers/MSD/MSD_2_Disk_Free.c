
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

SEC_CODE void MSD_Disk_Free( struct USBBase *usbbase UNUSED, struct MSDDisk *msddisk UNUSED )
{
	TASK_NAME_ENTER( "MSD_Disk_Free" );
	USBDEBUG( "MSD : MSD_Disk_Free" );

	// --


	// --

	TASK_NAME_LEAVE();
}

// --
