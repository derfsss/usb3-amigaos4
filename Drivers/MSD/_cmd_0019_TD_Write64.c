
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
// Handles both TD_WRITE64 (0x0019) and NSCMD_TD_WRITE64 (0xC001) -- the
// two commands carry identical semantics (64-bit byte offset split
// across io_Offset/io_Actual). Mirrors _cmd_C000_NSCmd_Read64.
#if 1

SEC_CODE S32 MSD_Cmd_0019_TD_Write64( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct DoBulkResult dbr;
struct MSDDisk *msddisk;
U64 blk_blocks;
U64 blk_start;
U32 blk_shift;
U32 blk_mask;
U64 offset;
U64 length;
U32 actual;
S32 reply;
S32 stat;

	TASK_NAME_ENTER( "MSD : _cmd_0019_TD_Write64" );
	USBERROR( "MSD : _cmd_0019_TD_Write64" );

	reply = TRUE;
	msddisk = (PTR) ioreq->io_Unit;

	// --
	// Validate Input

	if (( msddev->msddev_Detached )
	||	( ! msddisk )
	||	( ! msddisk->msddisk_Block_Size )		// Check it has been set
	||	( ! ioreq->io_Data )
	||	( ! ioreq->io_Length ))
	{
		/**/ if ( msddev->msddev_Detached )
		{
			USBERROR( "MSD : _cmd_0019_TD_Write64 : Device Detached" );
		}
		else if ( ! ioreq->io_Data )
		{
			USBERROR( "MSD : _cmd_0019_TD_Write64 : io_Data NULL Pointer" );
		}
		else if ( ! ioreq->io_Length )
		{
			USBERROR( "MSD : _cmd_0019_TD_Write64 : ioreq Zero Length" );
		}
		else if ( ! msddisk )
		{
			USBERROR( "MSD : _cmd_0019_TD_Write64 : invalid IOReq Unit" );
		}
		else if ( ! msddisk->msddisk_Block_Size )
		{
			USBERROR( "MSD : _cmd_0019_TD_Write64 : invalid Block Size" );
		}

		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

	blk_mask	= msddisk->msddisk_Block_Mask;
	blk_shift	= msddisk->msddisk_Block_Shift;

	// --
	// Enforce alignment (simplest + fastest)

	offset  = ( (U64) ioreq->io_Actual << 32 );
	offset |= ( (U64) ioreq->io_Offset <<  0 );
	length  = ( (U64) ioreq->io_Length );

	if (( offset & blk_mask ) || ( length & blk_mask ))
	{
		USBERROR( "MSD : TD_Write64 : Unaligned request (off=%llu len=%lu bs=%lu)", offset, length, msddisk->msddisk_Block_Size );
		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

	blk_start	= offset >> blk_shift;
	blk_blocks	= length >> blk_shift;

	if (( blk_start + blk_blocks ) > ( msddisk->msddisk_TotalBlocks ))
	{
		USBERROR( "MSD : TD_Write64 : OOB" );
		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

	actual = 0;

	if (( blk_start  > 0xFFFFFFFFULL )
	||	( blk_blocks > 0x0000FFFFUL ))
	{
		USBERROR( "MSD : TD_Write64 : Using MSD_SCSI_Write16" );

		stat = MSD_SCSI_Write16(
			usbbase,
			msddisk,
			& dbr,
			blk_start,
			(U32) blk_blocks,
			ioreq->io_Data,
			& actual
		);
	}
	else
	{
		USBERROR( "MSD : TD_Write64 : Using MSD_SCSI_Write10" );

		stat = MSD_SCSI_Write10(
			usbbase,
			msddisk,
			& dbr,
			(U32) blk_start,
			(U32) blk_blocks,
			ioreq->io_Data,
			& actual
		);
	}

	// Report actual tranfered even on fail
	ioreq->io_Actual = actual;

	USBERROR( "MSD : TD_Write64() okay : io_Actual %lu, csw Actual %ld", actual, dbr.csw_DataActual );

	if ( ! stat )
	{
		USBERROR( "MSD : TD_Write64() failed" );
		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

bailout:

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
