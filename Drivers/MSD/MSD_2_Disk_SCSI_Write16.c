
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

SEC_CODE S32 MSD_SCSI_Write16(
	struct USBBase *usbbase,
	struct MSDDisk *msddisk,
	struct DoBulkResult *dbr,
	U64 blk_start,
	U32 blk_blocks,
	PTR mem,
	U32 *actual_ptr )
{
struct DoBulkResult dbr_dummy;
struct DoBulkStruct dbs;
U64 got_blocks;
U64 max_blocks;
U64 got_bytes;
U64 max_bytes;
U32 blk_shift;
U32 blk_size;
U32 blk_mask;
U64 remaining;
U64 blocks;
S32 retval;
U32 actual;
U64 start;
U64 len;

	TASK_NAME_ENTER( "MSD_SCSI_Write16" );
	USBERROR( "MSD : MSD_SCSI_Write16" );

	actual = 0;
	retval = FALSE;

	if ( ! dbr )
	{
		dbr = & dbr_dummy;
	}

	blk_size  = msddisk->msddisk_Block_Size;
	blk_mask  = msddisk->msddisk_Block_Mask;
	blk_shift = msddisk->msddisk_Block_Shift;

	// Sanity on block size
	if (( ! blk_size ) || ( blk_size & blk_mask ))
	{
		// Not a power of two (or Zero)
		USBERROR( "MSD : MSD_SCSI_Write16 : bad Block Size %lu", blk_size );
		goto bailout;
	}

	// Set max 5x 20kb ehci buffers
	max_bytes	= 5 * 20 * 1024;
	max_blocks	= max_bytes >> blk_shift;
	max_blocks	= MIN( max_blocks, 0xffff );
	max_blocks	= MAX( max_blocks, 0x0001 );

	remaining	= blk_blocks;
	start		= blk_start;

	// --

	MEM_SET( & dbs, 0, sizeof( dbs ));

	dbs.dbs_Flags			= DOBULKF_DIR_OUT;
	dbs.dbs_Result			= dbr;
	dbs.dbs_Command_Length	= 16;
	dbs.cmd[ 0]	= SCSI_DA_WRITE_16;		// (0x8A)

	while( TRUE )
	{
		if ( ! remaining )
		{
			break;
		}

		blocks = MIN( max_blocks, remaining );
		len = blocks * blk_size;

		dbs.dbs_Data			= mem;
		dbs.dbs_Data_Length		= len;

		dbs.cmd[ 2]	= ( start  >> 56 ) & 0xff;
		dbs.cmd[ 3]	= ( start  >> 48 ) & 0xff;
		dbs.cmd[ 4]	= ( start  >> 40 ) & 0xff;
		dbs.cmd[ 5]	= ( start  >> 32 ) & 0xff;
		dbs.cmd[ 6]	= ( start  >> 24 ) & 0xff;
		dbs.cmd[ 7]	= ( start  >> 16 ) & 0xff;
		dbs.cmd[ 8]	= ( start  >>  8 ) & 0xff;
		dbs.cmd[ 9]	= ( start  >>  0 ) & 0xff;
		dbs.cmd[10]	= ( blocks >> 24 ) & 0xff;
		dbs.cmd[11]	= ( blocks >> 16 ) & 0xff;
		dbs.cmd[12]	= ( blocks >>  8 ) & 0xff;
		dbs.cmd[13]	= ( blocks >>  0 ) & 0xff;

		if ( ! MSD_Disk_DoBulk( usbbase, msddisk, & dbs ))
		{
			USBERROR( "MSD_SCSI_Write16() failed" );
			goto bailout;
		}

		if ( ! dbr->csw_Valid )
		{
			USBERROR( "WRITE16: invalid CSW" );
			goto bailout;
		}

		// If the device claims PASS, demand exact length
		if (( dbr->csw_Status == MSDCSW_Status_CmdPassed ) && ( dbr->csw_DataActual != len ))
		{
			USBERROR( "WRITE16: good CSW but short data (want=%lu got=%lu)", (U32) len, (U32) dbr->csw_DataActual );
			goto bailout;
		}

		// If it failed, stop here; caller can AUTOSENSE if needed
		if ( dbr->csw_Status == MSDCSW_Status_CmdFailed )
		{
			USBERROR( "WRITE16: cmd failed; residue=%lu", (U32)( len - dbr->csw_DataActual ));
			goto bailout;
		}

		got_bytes = dbr->csw_DataActual;

		if ( got_bytes & blk_mask )
		{
			USBERROR( "WRITE16: partial block bytes %lu", (U32) got_bytes );
			goto bailout;
		}

		got_blocks = got_bytes >> blk_shift;

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
