
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

SEC_CODE S32 MSD_SCSI_ReadCapacity10(
	struct USBBase *usbbase,
	struct MSDDisk *msddisk,
	struct DoBulkResult *dbr,
	PTR mem,
	U32 len )
{
struct DoBulkStruct dbs;
S32 retval;

	TASK_NAME_ENTER( "MSD_SCSI_ReadCapacity10" );
	USBERROR( "MSD : MSD_SCSI_ReadCapacity10" );

	retval = FALSE;

	// --
	MEM_SET( & dbs, 0, sizeof( dbs ));
	dbs.dbs_Flags			= DOBULKF_DIR_IN | DOBULKF_SHORT_OK;
	dbs.dbs_Result			= dbr;
	dbs.dbs_Command_Length	= 10;
	dbs.dbs_Data			= mem;
	dbs.dbs_Data_Length		= len;
	dbs.cmd[ 0]	= SCSI_DA_READ_CAPACITY;	// (0x25)
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
		USBERROR( "MSD_SCSI_ReadCapacity10() failed" );
		goto bailout;
	}

	#if 0

{
U32 data[2];
U32 lba;
U32 size;

	MEM_COPY( mem, data, 8 );

	lba		= BE_SWAP32( data[0] );
	size	= BE_SWAP32( data[1] );

	usbbase->usb_IExec->DebugPrintF( "Blocks ...... : %08lx %ld\n", lba, lba );
	usbbase->usb_IExec->DebugPrintF( "Block Size .. : %08lx %ld\n", size, size );
}

	#endif

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();
	return( retval );
}
