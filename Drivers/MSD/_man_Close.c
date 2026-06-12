
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

SEC_CODE PTR MSD_Manager_Close( struct DeviceManagerInterface *Self, struct IORequest *ioreq )
{
struct USBBase *usbbase;
struct MSDBase *MSDBase;
struct MSDDisk *msddisk;

	MSDBase = (PTR) Self->Data.LibBase;
	usbbase = MSDBase->msdbase_usbbase;

	TASK_NAME_ENTER( "MSD : MSD_Manager_Close" );
	USBERROR( "MSD : MSD_Manager_Close" );

	SEMAPHORE_OBTAIN( & MSDBase->msdbase_MSDDisk_Semaphore );

	// Adjust usb3disk.device open count
	MSDBase->msdbase_Library.lib_OpenCnt--;

	// Adjust msd/unit open count
	msddisk = (PTR) ioreq->io_Unit;
	msddisk->msddisk_OpenCnt--;

	// Make sure IOReq will not be used again
	ioreq->io_Unit	 = NULL;
	ioreq->io_Device = NULL;

	// If the device is detaching, its task sits in the Stopping loop
	// waiting for the open counts to drain -- wake it so it can
	// re-check and finish its teardown
	if (( msddisk->msddisk_MSDDev )
	&&	( msddisk->msddisk_MSDDev->msddev_Detached ))
	{
		struct MsgPort *port = & msddisk->msddisk_MSDDev->msddev_MsgPort_Begin.ump_MsgPort;

		if ( port->mp_SigTask )
		{
			usbbase->usb_IExec->Signal( port->mp_SigTask, 1UL << port->mp_SigBit );
		}
	}

	SEMAPHORE_RELEASE( & MSDBase->msdbase_MSDDisk_Semaphore );

	TASK_NAME_LEAVE();
	return( NULL );
}

// --
