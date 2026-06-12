
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

SEC_CODE S32 MSD_SCSI_TestUnitReady(
	struct USBBase *usbbase,
	struct MSDDisk *msddisk,
	struct DoBulkResult *dbr )
{
struct DoBulkStruct dbs;
S32 retval;

	TASK_NAME_ENTER( "MSD_SCSI_TestUnitReady" );
	USBERROR( "MSD : MSD_SCSI_TestUnitReady" );

	retval = FALSE;

	// --
	MEM_SET( & dbs, 0, sizeof( dbs ));
	dbs.dbs_Flags			= DOBULKF_DIR_OUT;
	dbs.dbs_Result			= dbr;
	dbs.dbs_Command_Length	= 6;
//	dbs.cmd[ 0]	= 0x00;		// Test Unit Ready (0x00)
//	dbs.cmd[ 1]	= 0;
//	dbs.cmd[ 2]	= 0;
//	dbs.cmd[ 3]	= 0;
//	dbs.cmd[ 4]	= 0;
//	dbs.cmd[ 5]	= 0;
//	dbs.cmd[ 6]	= 0;
//	dbs.cmd[ 7]	= 0;
//	dbs.cmd[ 8]	= 0;
//	dbs.cmd[ 9]	= 0;
//	dbs.cmd[10]	= 0;
//	dbs.cmd[11]	= 0;
//	dbs.cmd[12]	= 0;
//	dbs.cmd[13]	= 0;
//	dbs.cmd[14]	= 0;
//	dbs.cmd[15]	= 0;

	// --
	if ( ! MSD_Disk_DoBulk( usbbase, msddisk, & dbs ))
	{
		USBERROR( "MSD_SCSI_TestUnitReady() failed" );
		goto bailout;
	}

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();
	return( retval );
}
