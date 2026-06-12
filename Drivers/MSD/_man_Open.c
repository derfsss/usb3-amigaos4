
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

SEC_CODE S32 MSD_Manager_Open(
	struct DeviceManagerInterface *Self,
	struct IORequest *ioreq,
	U32 unit_number,
	U32 flags UNUSED )
{
struct USBBase *usbbase;
struct MSDBase *MSDBase;
struct MSDDisk *msddisk;

	MSDBase = (PTR) Self->Data.LibBase;
	usbbase = MSDBase->msdbase_usbbase;

	TASK_NAME_ENTER( "MSD : MSD_Manager_Open" );
	USBERROR( "MSD : MSD_Manager_Open" );

	SEMAPHORE_OBTAIN( & MSDBase->msdbase_MSDDisk_Semaphore );

	if ( unit_number < MAX_MSD_UNITS )
	{
		msddisk = MSDBase->msdbase_MSDDisk_Units[ unit_number ];
	}
	else
	{
		msddisk = NULL;
	}

	// Refuse opens on a unit whose device is already detaching
	if (( msddisk )
	&&	( msddisk->msddisk_MSDDev )
	&&	( msddisk->msddisk_MSDDev->msddev_Detached ))
	{
		USBERROR( "MSD : MSD_Manager_Open : Unit %ld is detached", unit_number );
		msddisk = NULL;
	}

	if (( msddisk )
	&&	( ioreq->io_Message.mn_Length )
	&&	( ioreq->io_Message.mn_Length >= sizeof( struct IORequest )))
	{
		// okay

		MSDBase->msdbase_Library.lib_Flags &= ~LIBF_DELEXP;
		MSDBase->msdbase_Library.lib_OpenCnt++;

		ioreq->io_Message.mn_Node.ln_Type = NT_REPLYMSG;
		ioreq->io_Unit = (PTR) msddisk;
		ioreq->io_Error = 0;

		msddisk->msddisk_OpenCnt++;
	}
	else
	{
		// error

		ioreq->io_Error = IOERR_OPENFAIL;
		ioreq->io_Device = NULL;
	}

	SEMAPHORE_RELEASE( & MSDBase->msdbase_MSDDisk_Semaphore );

	TASK_NAME_LEAVE();
	return(	ioreq->io_Error );
}

// --
