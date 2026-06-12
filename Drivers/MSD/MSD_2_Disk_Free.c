
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
// Final teardown of one LUN. The disk must already be detached
// (denounced) and should have no openers left -- the caller checks
// that. Any change/status requests still parked here belong to clients
// that did not clean up; abort them so no IORequest is leaked.

SEC_CODE void MSD_Disk_Free( struct USBBase *usbbase, struct MSDDisk *msddisk )
{
struct MSDBase *msdbase;
struct IOStdReq *ioreq;

	TASK_NAME_ENTER( "MSD_Disk_Free" );
	USBDEBUG( "MSD : MSD_Disk_Free" );

	// --

	SEMAPHORE_OBTAIN( & msddisk->msddisk_MSDUnit_Semaphore );

	while(( ioreq = (PTR) NODE_REMHEAD( & msddisk->msddisk_MSDUnit_ChangeList )))
	{
		USBERROR( "MSD : MSD_Disk_Free : aborting parked ChangeInt IOReq %p", ioreq );

		ioreq->io_Error = IOERR_ABORTED;
		MSGPORT_REPLYMSG( ioreq );
	}

	while(( ioreq = (PTR) NODE_REMHEAD( & msddisk->msddisk_MSDUnit_StatList )))
	{
		USBERROR( "MSD : MSD_Disk_Free : aborting parked StatCallBack IOReq %p", ioreq );

		ioreq->io_Error = IOERR_ABORTED;
		MSGPORT_REPLYMSG( ioreq );
	}

	SEMAPHORE_RELEASE( & msddisk->msddisk_MSDUnit_Semaphore );

	// --
	// Release the unit number. Safe to reuse only because
	// DenounceDevice already ran (mounter autodoc: a unit must not be
	// reused until it has been denounced).

	msdbase = msddisk->msddisk_MSDBase;

	if ( msdbase )
	{
		SEMAPHORE_OBTAIN( & msdbase->msdbase_MSDDisk_Semaphore );

		if (( msddisk->msddisk_UnitNr < MAX_MSD_UNITS )
		&&	( msdbase->msdbase_MSDDisk_Units[ msddisk->msddisk_UnitNr ] == msddisk ))
		{
			msdbase->msdbase_MSDDisk_Units[ msddisk->msddisk_UnitNr ] = NULL;
		}

		SEMAPHORE_RELEASE( & msdbase->msdbase_MSDDisk_Semaphore );
	}

	// --

	MEM_FREEVEC( msddisk );

	TASK_NAME_LEAVE();
}

// --
