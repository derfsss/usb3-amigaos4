
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

SEC_CODE S32 MSD_SCSI_Read16( 
	struct USBBase *usbbase,
	struct MSDDisk *msddisk,
	struct DoBulkResult *dbr,
	U64 blk_start,
	U32 blk_blocks, 
	PTR mem,
	U32 *actual_ptr )
{
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

	TASK_NAME_ENTER( "MSD_SCSI_Read16" );
	USBERROR( "MSD : MSD_SCSI_Read16" );

	actual = 0;
	retval = FALSE;

	blk_size  = msddisk->msddisk_Block_Size;
	blk_mask  = msddisk->msddisk_Block_Mask;
	blk_shift = msddisk->msddisk_Block_Shift;

	// Sanity on block size
//	if (( ! blk_size ) || (( blk_size & ( blk_size - 1 )))) 
	if (( ! blk_size ) || ( blk_size & blk_mask )) 
	{
		// Not a power of two (or Zero)
		USBERROR( "MSD : MSD_SCSI_Read16 : bad Block Size %lu", blk_size );
		// hmm maybe try and do a Read Geometry here?
		goto bailout;
	}

	// Set max 5x 20kb ehci buffers
	max_bytes	= 5 * 20 * 1024;
//	max_blocks	= max_bytes / blk_size;
	max_blocks	= max_bytes >> blk_shift;
	max_blocks	= MIN( max_blocks, 0xffff );
	max_blocks	= MAX( max_blocks, 0x0001 );
	remaining	= blk_blocks;
	start		= blk_start;

	// --

	// --
	MEM_SET( & dbs, 0, sizeof( dbs ));
	dbs.dbs_Flags			= DOBULKF_DIR_IN | DOBULKF_SHORT_OK;
	dbs.dbs_Command_Length	= 16;
	dbs.dbs_Result			= dbr;
	dbs.cmd[ 0]	= SCSI_DA_READ_16;		// (0x88)
//	dbs.cmd[ 1]	= 0;					// flags
//	dbs.cmd[ 2]	= ( start  >> 56 ) & 0xff;
//	dbs.cmd[ 3]	= ( start  >> 48 ) & 0xff;
//	dbs.cmd[ 4]	= ( start  >> 40 ) & 0xff;
//	dbs.cmd[ 5]	= ( start  >> 32 ) & 0xff;
//	dbs.cmd[ 6]	= ( start  >> 24 ) & 0xff;
//	dbs.cmd[ 7]	= ( start  >> 16 ) & 0xff;
//	dbs.cmd[ 8]	= ( start  >>  8 ) & 0xff;
//	dbs.cmd[ 9]	= ( start  >>  0 ) & 0xff;
//	dbs.cmd[10]	= ( blocks >> 24 ) & 0xff;
//	dbs.cmd[11]	= ( blocks >> 16 ) & 0xff;
//	dbs.cmd[12]	= ( blocks >>  8 ) & 0xff;
//	dbs.cmd[13]	= ( blocks >>  0 ) & 0xff;
//	dbs.cmd[14]	= 0;					// group
//	dbs.cmd[15]	= 0;					// control

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
//		dbs.cmd[ 0]	= SCSI_DA_READ_16;		// (0x88)
//		dbs.cmd[ 1]	= 0;					// flags
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
//		dbs.cmd[14]	= 0;					// group
//		dbs.cmd[15]	= 0;					// control

		if ( ! MSD_Disk_DoBulk( usbbase, msddisk, & dbs ))
		{
			USBERROR( "MSD_SCSI_Read16() failed" );
			goto bailout;
		}

		if ( ! dbr->csw_Valid )
		{
			USBERROR( "READ16: invalid CSW" ); 
			goto bailout;
		}

		// If the device claims PASS, demand exact length
		if (( dbr->csw_Status == MSDCSW_Status_CmdPassed ) && ( dbr->csw_DataActual != len ))
		{
			USBERROR( "READ16: good CSW but short data (want=%lu got=%lu)", (U32) len, (U32) dbr->csw_DataActual );
			goto bailout; // treat as error
		}

		// If it failed, stop here; caller can AUTOSENSE if needed
		if ( dbr->csw_Status == MSDCSW_Status_CmdFailed ) 
		{
			USBERROR( "READ16: cmd failed; residue=%lu", (U32)( len - dbr->csw_DataActual ));
			goto bailout;
		}

		got_bytes = dbr->csw_DataActual;

//		if ( got_bytes % blk_size )
		if ( got_bytes & blk_mask )
		{
			USBERROR( "READ16: partial block bytes %lu", got_bytes ); 
			goto bailout; 
		}

//		got_blocks = got_bytes / blk_size;
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
