
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

SEC_CODE void MSD_Disk_Detach( struct USBBase *usbbase, struct MSDDisk *msddisk )
{
struct MSDDevice *msddev;

	TASK_NAME_ENTER( "MSD_Disk_Detach" );
	USBERROR( "MSD : MSD_Disk_Detach" );

	//	--

	msddev = msddisk->msddisk_MSDDev;
	msddev->msddev_Detached = TRUE;

	if ( msddisk->msddisk_Announced )
	{
		usbbase->usb_IMounter->DenounceDevice( MSD_Device_Name, msddisk->msddisk_UnitNr );
		msddisk->msddisk_Announced = FALSE;
	}

	// --

	TASK_NAME_LEAVE();
}

// --
