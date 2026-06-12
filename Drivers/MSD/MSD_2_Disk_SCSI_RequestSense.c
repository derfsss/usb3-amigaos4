
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

SEC_CODE S32 MSD_SCSI_RequestSense(
	struct USBBase *usbbase,
	struct MSDDisk *msddisk,
	struct DoBulkResult *dbr,
	PTR mem,
	U32 reqlen )
{
struct DoBulkStruct dbs;
S32 retval;
U32 len;

	TASK_NAME_ENTER( "MSD_SCSI_RequestSense" );
	USBERROR( "MSD : MSD_SCSI_RequestSense" );

	retval = FALSE;

	len = reqlen;

	if (( len < 4 ) || ( 255 < len ))
	{
		len = MIN( reqlen, 255 );
		len = MAX( len, 4 );

		USBERROR( "MSD : MSD_SCSI_RequestSense : Clamping length %ld -> %ld", reqlen, len );
	}

	// --
	MEM_SET( & dbs, 0, sizeof( dbs ));
	dbs.dbs_Flags			= DOBULKF_DIR_IN | DOBULKF_SHORT_OK;
	dbs.dbs_Result			= dbr;
	dbs.dbs_Command_Length	= 6;
	dbs.dbs_Data			= mem;
	dbs.dbs_Data_Length		= len;
	dbs.cmd[ 0]	= 0x03;				// REQUEST SENSE;
//	dbs.cmd[ 1]	= 0;				// Desc bit & reserved
//	dbs.cmd[ 2]	= 0;				// Reserved
//	dbs.cmd[ 3]	= 0;				// Reserved
	dbs.cmd[ 4]	= ( len & 0xff );	// Length 4-255 bytes
//	dbs.cmd[ 5]	= 0;				// Control
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
		USBERROR( "MSD_SCSI_RequestSense() failed" );
		goto bailout;
	}

	#if 1
{
	U8 *sense = (U8 *)mem;

	if (( sense[0] & 0x7F) == 0x70 )
	{
		U8 senseKey	= sense[2] & 0x0F;
		U8 asc		= sense[12];
		U8 ascq		= sense[13];

		USBDEBUG( "MSD : Sense Key: $%02lx  ASC: $%02lx  ASCQ: $%02lx", senseKey, asc, ascq );
	}
	else
	{
		USBDEBUG( "MSD : Sense Key: Failed" );
	}
}
	#endif

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();
	return( retval );
}

// --
