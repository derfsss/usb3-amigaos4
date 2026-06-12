
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
#if 1

SEC_CODE S32 MSD_Cmd_0003_CMD_Write( struct USBBase *usbbase, struct MSDDevice *msddev, struct IOStdReq *ioreq )
{
// struct DoBulkResult dbr;
struct MSDDisk *msddisk;
U64 blk_blocks;
U64 blk_start;
U32 actual;
S32 reply;
S32 stat;

	TASK_NAME_ENTER( "MSD : CMD_Write" );
//	USBERROR( "MSD : _cmd_0003_CMD_Write : Offset $%08lx : %lu bytes", ioreq->io_Offset, ioreq->io_Length );

	reply = TRUE;
	msddisk = (PTR) ioreq->io_Unit;

//	USBERROR( "MSD : io_Data .......... : %p", ioreq->io_Data );
//	USBERROR( "MSD : io_Length ........ : %ld", ioreq->io_Length );
//	USBERROR( "MSD : io_Offset ........ : $%08lx", ioreq->io_Offset );

	// --
	// Validate Input

	if ((	msddev->msddev_Detached )
	||	( ! msddisk )
	||	( ! msddisk->msddisk_Block_Size )
	||	( ! ioreq->io_Data ) 
	||	( ! ioreq->io_Length )
//	||	(	ioreq->io_Offset % msddisk->msddisk_Block_Size )
	||	(	ioreq->io_Offset & msddisk->msddisk_Block_Mask )
//	||	(	ioreq->io_Length % msddisk->msddisk_Block_Size ))
	||	(	ioreq->io_Length & msddisk->msddisk_Block_Mask ))
	{
			/**/ if ( msddev->msddev_Detached )
		{
			USBERROR( "MSD : _cmd_0003_CMD_Write : Device Detached" );
		}
		else if ( ! msddisk )
		{
			USBERROR( "MSD : _cmd_0003_CMD_Write : invalid IOReq Unit" );
		}
		else if ( ! msddisk->msddisk_Block_Size )
		{
			USBERROR( "MSD : _cmd_0003_CMD_Write : invalid Block Size" );
		}
		else if ( ! ioreq->io_Data )
		{
			USBERROR( "MSD : _cmd_0003_CMD_Write : io_Data NULL Pointer" );
		}
		else if ( ! ioreq->io_Length )
		{
			USBERROR( "MSD : _cmd_0003_CMD_Write : io_Length Zero Length" );
		}
//		else if ( ! ioreq->io_Offset % msddisk->msddisk_Block_Size )
		else if ( ! ( ioreq->io_Offset & msddisk->msddisk_Block_Mask ))
		{
			USBERROR( "MSD : _cmd_0003_CMD_Write : invalid io_Offset (%lu)", ioreq->io_Offset );
		}
//		else if ( ! ioreq->io_Length % msddisk->msddisk_Block_Size )
		else if ( ! ( ioreq->io_Length & msddisk->msddisk_Block_Mask ))
		{
			USBERROR( "MSD : _cmd_0003_CMD_Write : invalid io_Length (%lu)", ioreq->io_Length );
		}

		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

//	blk_start	= ioreq->io_Offset / msddisk->msddisk_Block_Size;
	blk_start	= ioreq->io_Offset >> msddisk->msddisk_Block_Shift;
//	blk_blocks	= ioreq->io_Length / msddisk->msddisk_Block_Size;
	blk_blocks	= ioreq->io_Length >> msddisk->msddisk_Block_Shift;

	if (( blk_start + blk_blocks ) > ( msddisk->msddisk_TotalBlocks ))
	{
		USBERROR( "MSD : CMD_Write : OOB" );
		ioreq->io_Error = IOERR_BADLENGTH;
		// hmm maybe clamp instead of error
		goto bailout;
	}

	// --

	actual = 0;

	stat = MSD_SCSI_Write10( 
		usbbase,
		msddisk,
		NULL,	// & dbr, 
		blk_start,
		blk_blocks,
		ioreq->io_Data,
		& actual
	);

	// Report actual tranfered even on fail
	ioreq->io_Actual = actual;

//	USBERROR( "MSD : CMD_Write() okay : io_Actual %lu, csw Actual %ld", actual, dbr.csw_DataActual );
//	USBERROR( "MSD : CMD_Write() okay : io_Actual %lu", actual );

	if ( ! stat )
	{
		USBERROR( "MSD : CMD_Write() failed" );
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
