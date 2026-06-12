
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
// Tell everyone watching this unit that the media situation changed.
//
// TD_ADDCHANGEINT handlers are Cause()'d and stay parked until
// TD_REMCHANGEINT (per the trackdisk.device autodoc). Queued
// NSCMD_TD_ADDSTATCALLBACK requests are replied once with io_Actual
// non-zero (status changed / no media); the caller re-sends the request
// if it wants further notifications.

SEC_CODE void MSD_Disk_FireChangeInts( struct USBBase *usbbase, struct MSDDisk *msddisk )
{
struct IOStdReq *ioreq;
PTR node;

	TASK_NAME_ENTER( "MSD_Disk_FireChangeInts" );
	USBERROR( "MSD : MSD_Disk_FireChangeInts : Unit %ld", msddisk->msddisk_UnitNr );

	SEMAPHORE_OBTAIN( & msddisk->msddisk_MSDUnit_Semaphore );

	msddisk->msddisk_DiskChangeCount++;

	node = msddisk->msddisk_MSDUnit_ChangeList.uh_Head;

	while( node )
	{
		ioreq = (PTR) node;

		if ( ioreq->io_Data )
		{
			usbbase->usb_IExec->Cause( ioreq->io_Data );
		}

		node = Node_Next( node );
	}

	while(( ioreq = (PTR) NODE_REMHEAD( & msddisk->msddisk_MSDUnit_StatList )))
	{
		ioreq->io_Error  = 0;
		ioreq->io_Actual = 1;

		MSGPORT_REPLYMSG( ioreq );
	}

	SEMAPHORE_RELEASE( & msddisk->msddisk_MSDUnit_Semaphore );

	TASK_NAME_LEAVE();
}

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
		msddisk->msddisk_Announced = FALSE;

		// Filesystems first, then the Mounter pulls the volume
		MSD_Disk_FireChangeInts( usbbase, msddisk );

		usbbase->usb_IMounter->DenounceDevice( MSD_Device_Name, msddisk->msddisk_UnitNr );
	}

	// --

	TASK_NAME_LEAVE();
}

// --
// Detach every LUN of this device. Called when the device leaves the
// bus (or the stack shuts down) -- this is what makes volumes disappear
// on unplug instead of on the next failed I/O.

SEC_CODE void MSD_Device_Detach( struct USBBase *usbbase, struct MSDDevice *msddev )
{
struct MSDDisk *msddisk;
PTR node;

	TASK_NAME_ENTER( "MSD_Device_Detach" );
	USBERROR( "MSD : MSD_Device_Detach" );

	msddev->msddev_Detached = TRUE;

	node = msddev->msddev_MSDDisk_Header.uh_Head;

	while( node )
	{
		msddisk = (PTR) node;

		MSD_Disk_Detach( usbbase, msddisk );

		node = Node_Next( node );
	}

	TASK_NAME_LEAVE();
}

// --
