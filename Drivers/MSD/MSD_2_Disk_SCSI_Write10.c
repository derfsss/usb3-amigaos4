
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

SEC_CODE S32 MSD_SCSI_Write10( 
	struct USBBase *usbbase,
	struct MSDDisk *msddisk,
	struct DoBulkResult *dbr,
	U32 blk_start,
	U32 blk_blocks, 
	PTR mem,
	U32 *actual_ptr )
{
struct DoBulkResult dbr_dummy;
struct DoBulkStruct dbs;
U32 got_blocks;
U32 max_blocks;
U32 got_bytes;
U32 max_bytes;
U32 blk_size;
U32 remaining;
U32 blocks;
S32 retval;
U32 actual;
U32 start;
U32 len;

	TASK_NAME_ENTER( "MSD_SCSI_Write10" );
//	USBERROR( "MSD : MSD_SCSI_Write10" );

	actual = 0;
	retval = FALSE;

	if ( ! dbr )
	{
		dbr = & dbr_dummy;
	}

	blk_size = msddisk->msddisk_Block_Size;

	// Sanity on block size
	if (( ! blk_size ) || (( blk_size & ( blk_size - 1 )))) 
	{
		// Not a power of two (or Zero)
		USBERROR( "MSD : MSD_SCSI_Write10 : bad Block Size %lu", blk_size );
		// hmm maybe try and do a Read Geometry here?
		goto bailout;
	}

	// Set max 5x 20kb ehci buffers
	max_bytes	= 5 * 20 * 1024;
	max_blocks	= max_bytes / blk_size;
	max_blocks	= MAX( max_blocks, 1 );
	max_blocks	= MIN( max_blocks, 0xffff );
	remaining	= blk_blocks;
	start		= blk_start;

	// --
	MEM_SET( & dbs, 0, sizeof( dbs ));
	dbs.dbs_Flags			= DOBULKF_DIR_OUT;
	dbs.dbs_Result			= dbr;
	dbs.dbs_Command_Length	= 10;
	dbs.cmd[ 0]	= SCSI_DA_WRITE_10;		// (0x2A)
//	dbs.cmd[ 1]	= 0;					// flags
//	dbs.cmd[ 2]	= ( start >> 24 ) & 0xff;
//	dbs.cmd[ 3]	= ( start >> 16 ) & 0xff;
//	dbs.cmd[ 4]	= ( start >>  8 ) & 0xff;
//	dbs.cmd[ 5]	= ( start >>  0 ) & 0xff;
//	dbs.cmd[ 6]	= 0;					// group nr
//	dbs.cmd[ 7]	= ( blocks >> 8 ) & 0xff;
//	dbs.cmd[ 8]	= ( blocks >> 0 ) & 0xff;
//	dbs.cmd[ 9]	= 0;					// control
//	dbs.cmd[10]	= 0;
//	dbs.cmd[11]	= 0;
//	dbs.cmd[12]	= 0;
//	dbs.cmd[13]	= 0;
//	dbs.cmd[14]	= 0;
//	dbs.cmd[15]	= 0;

	while( TRUE )
	{
//		USBERROR( "Mem : %p : Start $%08lx blk : Remaning %4lu blks : CurActual %lu bytes", mem, start, remaining, actual );

		if ( ! remaining )
		{
			break;
		}

		blocks = MIN( max_blocks, remaining );
		len = blocks * blk_size;

		dbs.dbs_Data			= mem;
		dbs.dbs_Data_Length		= len;
//		dbs.cmd[ 0]	= SCSI_DA_WRITE_10;		// (0x2A)
//		dbs.cmd[ 1]	= 0;					// flags
		dbs.cmd[ 2]	= ( start >> 24 ) & 0xff;
		dbs.cmd[ 3]	= ( start >> 16 ) & 0xff;
		dbs.cmd[ 4]	= ( start >>  8 ) & 0xff;
		dbs.cmd[ 5]	= ( start >>  0 ) & 0xff;
//		dbs.cmd[ 6]	= 0;					// group nr
		dbs.cmd[ 7]	= ( blocks >> 8 ) & 0xff;
		dbs.cmd[ 8]	= ( blocks >> 0 ) & 0xff;
//		dbs.cmd[ 9]	= 0;					// control
//		dbs.cmd[10]	= 0;
//		dbs.cmd[11]	= 0;
//		dbs.cmd[12]	= 0;
//		dbs.cmd[13]	= 0;
//		dbs.cmd[14]	= 0;
//		dbs.cmd[15]	= 0;

		if ( ! MSD_Disk_DoBulk( usbbase, msddisk, & dbs ))
		{
			USBERROR( "MSD_SCSI_Write10() failed" );
			goto bailout;
		}

		if ( ! dbr->csw_Valid )
		{
			USBERROR( "WRITE10: invalid CSW" ); 
			goto bailout;
		}

		// If the device claims PASS, demand exact length
		if (( dbr->csw_Status == MSDCSW_Status_CmdPassed ) && ( dbr->csw_DataActual != len ))
		{
			USBERROR( "WRITE10: good CSW but short data (want=%lu got=%lu)", (U32) len, (U32) dbr->csw_DataActual );
			goto bailout; // treat as error
		}

		// If it failed, stop here; caller can AUTOSENSE if needed
		if ( dbr->csw_Status == MSDCSW_Status_CmdFailed ) 
		{
			USBERROR( "WRITE10: cmd failed; residue=%lu", (U32)( len - dbr->csw_DataActual ));
			goto bailout;
		}

		got_bytes = dbr->csw_DataActual;

		if ( got_bytes % blk_size ) 
		{
			USBERROR( "WRITE10: partial block bytes %lu", got_bytes ); 
			goto bailout; 
		}

		got_blocks = got_bytes / blk_size;

		mem = (U8 *) mem + got_bytes ;
		start += got_blocks;
		actual += got_bytes;
		remaining -= got_blocks;
	}

	retval = TRUE;

bailout:

	if ( actual_ptr )
	{
		*actual_ptr = actual;
	}

	TASK_NAME_LEAVE();
	return( retval );
}

// --
