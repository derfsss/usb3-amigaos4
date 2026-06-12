
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

SEC_CODE void MSD_Device_Free( struct USBBase *usbbase, struct MSDDevice *msddev )
{
struct MSDDisk *msddisk;

	TASK_NAME_ENTER( "MSD_Device_Free" );
	USBDEBUG( "MSD : MSD_Device_Free" );

	// --

	if ( ! msddev )
	{
		goto bailout;
	}

	// Make sure every LUN is denounced and the watchers told, even if
	// we get here without going through the normal stopping sequence
	MSD_Device_Detach( usbbase, msddev );

	while(( msddisk = (PTR) NODE_REMHEAD( & msddev->msddev_MSDDisk_Header )))
	{
		if ( msddisk->msddisk_OpenCnt > 0 )
		{
			// Task is going away regardless; the unit table entry is
			// cleared so no new opens can reach the stale unit
			USBERROR( "MSD : MSD_Device_Free : Unit %ld still has %ld opener(s)",
				msddisk->msddisk_UnitNr, msddisk->msddisk_OpenCnt );
		}

		MSD_Disk_Free( usbbase, msddisk );
	}

	// Note: msddev itself (msgports, register resources) is still
	// released by the surrounding framework / task teardown; freeing
	// it here would race the stack's own cleanup.

bailout:

	TASK_NAME_LEAVE();
}

// --
