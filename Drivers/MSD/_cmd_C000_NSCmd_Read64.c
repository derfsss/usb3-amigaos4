
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

SEC_CODE S32 MSD_Cmd_C000_NSCmd_Read64( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct DoBulkResult dbr;
struct MSDDisk *msddisk;
U64 blk_blocks;
U64 blk_start;
U32 blk_shift;
//U32 blk_size;
U32 blk_mask;
U64 offset;
U64 length;
U32 actual;
S32 reply;
S32 stat;

	TASK_NAME_ENTER( "MSD : _cmd_C000_NSCmd_Read64" );
	USBERROR( "MSD : _cmd_C000_NSCmd_Read64" );

	reply = TRUE;
	msddisk = (PTR) ioreq->io_Unit;

//	USBERROR( "MSD : io_Data .......... : %p", ioreq->io_Data );
//	USBERROR( "MSD : io_Length ........ : %ld", ioreq->io_Length );
//	USBERROR( "MSD : io_Actual ........ : $%08lx", ioreq->io_Actual );
//	USBERROR( "MSD : io_Offset ........ : $%08lx", ioreq->io_Offset );

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
			USBERROR( "MSD : _cmd_C000_NSCmd_Read64 : Device Detached" );
		}
		else if ( ! ioreq->io_Data )
		{
			USBERROR( "MSD : _cmd_C000_NSCmd_Read64 : io_Data NULL Pointer" );
		}
		else if ( ! ioreq->io_Length )
		{
			USBERROR( "MSD : _cmd_C000_NSCmd_Read64 : ioreq Zero Length" );
		}
		else if ( ! msddisk )
		{
			USBERROR( "MSD : _cmd_C000_NSCmd_Read64 : invalid IOReq Unit" );
		}
		else if ( ! msddisk->msddisk_Block_Size )
		{
			USBERROR( "MSD : _cmd_C000_NSCmd_Read64 : invalid Block Size" );
		}

		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

//	blk_size	= msddisk->msddisk_Block_Size;
	blk_mask	= msddisk->msddisk_Block_Mask;
	blk_shift	= msddisk->msddisk_Block_Shift;

	// --
	// Enforce alignment (simplest + fastest)

	offset  = ( (U64) ioreq->io_Actual << 32 );
	offset |= ( (U64) ioreq->io_Offset <<  0 );
	length  = ( (U64) ioreq->io_Length );

//	if (( offset % blk_size ) || ( length % blk_size ))
	if (( offset & blk_mask ) || ( length & blk_mask ))
	{
		USBERROR( "MSD : NSCmd_Read64 : Unaligned request (off=%llu len=%lu bs=%lu)", offset, length, msddisk->msddisk_Block_Size );
		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

//	blk_start	= offset / blk_size;
	blk_start	= offset >> blk_shift;
//	blk_blocks	= length / blk_size;
	blk_blocks	= length >> blk_shift;

	if (( blk_start + blk_blocks ) > ( msddisk->msddisk_TotalBlocks ))
	{
		USBERROR( "MSD : NSCmd_Read64 : OOB" );
		ioreq->io_Error = IOERR_BADLENGTH;
		// hmm maybe clamp instead of error
		goto bailout;
	}

	// --

	actual = 0;

	if (( blk_start  > 0xFFFFFFFFULL )
	||	( blk_blocks > 0x0000FFFFUL ))
	{
		USBERROR( "MSD : NSCmd_Read64 : Using MSD_SCSI_Read16" );

		stat = MSD_SCSI_Read16( 
			usbbase,
			msddisk,
			& dbr, 
			blk_start,
			blk_blocks,
			ioreq->io_Data,
			& actual
		);
	}
	else
	{
		USBERROR( "MSD : NSCmd_Read64 : Using MSD_SCSI_Read10" );

		stat = MSD_SCSI_Read10(
			usbbase,
			msddisk,
			& dbr, 
			blk_start,
			blk_blocks,
			ioreq->io_Data,
			& actual
		);
	}

	// Report actual tranfered even on fail
	ioreq->io_Actual = actual;

	USBERROR( "MSD : NSCmd_Read64() okay : io_Actual %lu, csw Actual %ld", actual, dbr.csw_DataActual );

	if ( ! stat )
	{
		USBERROR( "MSD : NSCmd_Read64() failed" );
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
