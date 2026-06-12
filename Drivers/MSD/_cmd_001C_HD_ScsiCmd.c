
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"
#include "MSD.h"

// --
#if 1

struct __scsi_intern
{
	U8 cmd[16];
};

// --
// 0x25 : SCSI_DA_READ_CAPACITY

SEC_CODE static void __HDSCSI_ReadCap10( struct USBBase *usbbase, struct IOStdReq *ioreq, struct __scsi_intern *si UNUSED )
{
struct MSDDisk *msddisk;
struct SCSICmd *scsi;
U32 data[2];
U32 len;

	// This way we make sure we are working with same sizes
	// ie BlockSize have been adjusted/validated

	scsi	= (PTR) ioreq->io_Data;
	msddisk = (PTR) ioreq->io_Unit;


	if ( msddisk->msddisk_TotalBlocks > 0xFFFFFFFFULL )
	{
		data[0]	= BE_SWAP32( (U32) ( 0xFFFFFFFFUL ));
		data[1]	= BE_SWAP32( (U32) ( msddisk->msddisk_Block_Size ));
	}
	else
	{
		data[0]	= BE_SWAP32( (U32) ( msddisk->msddisk_TotalBlocks - 1 ));
		data[1]	= BE_SWAP32( (U32) ( msddisk->msddisk_Block_Size ));
	}

	len = MIN( scsi->scsi_Length, 8 );

	if (( scsi->scsi_Data ) && ( len ))
	{
		MEM_COPY( scsi->scsi_Data, data, len );
	}
	else
	{
		len = 0;
	}

	scsi->scsi_SenseActual	= 0;
	scsi->scsi_CmdActual	= scsi->scsi_CmdLength;		// What if Cmd is not 6 bytes??
	scsi->scsi_Status		= SCSI_GOOD;
	scsi->scsi_Actual		= len;
	ioreq->io_Actual		= len;
	ioreq->io_Error			= 0;
}

// --

SEC_CODE void __HDSCSI_Default( struct USBBase *usbbase, struct IOStdReq *ioreq, struct __scsi_intern *si UNUSED )
{
struct DoBulkStruct dbs;
struct DoBulkResult sbr;	// ReqSense Result
struct DoBulkResult dbr;	// Scsi Result
struct MSDDisk *msddisk;
struct SCSICmd *scsi;
U32 flags;
S32 stat;

	TASK_NAME_ENTER( "MSD : HD_ScsiCmd : Default" );
	USBERROR( "MSD : HD_ScsiCmd : Default" );

	scsi	= ioreq->io_Data;
	msddisk = (PTR) ioreq->io_Unit;


	// --
	// Map to DoBulk flags

	flags = 0;

	if (( scsi->scsi_Data ) && ( scsi->scsi_Length )) 
	{
		if ( scsi->scsi_Flags & SCSIF_READ ) 
		{
			flags |= DOBULKF_DIR_IN | DOBULKF_SHORT_OK;		// short IN is common for SCSI-direct
		} 
		else 
		{
			flags |= DOBULKF_DIR_OUT;
		}
	}

	if ( scsi->scsi_Flags & SCSIF_AUTOSENSE ) 
	{
		// informational; policy below
		flags |= DOBULKF_AUTOSENSE; 
	}

	scsi->scsi_Actual		= 0;
	scsi->scsi_CmdActual	= 0;
	scsi->scsi_SenseActual	= 0;
	scsi->scsi_Status		= 0x00;

	// --
	MEM_SET( & dbs, 0, sizeof( dbs ));
	MEM_COPY( scsi->scsi_Command, & dbs.cmd, scsi->scsi_CmdLength );
	dbs.dbs_Flags			= flags;
	dbs.dbs_Result			= & dbr;
	dbs.dbs_Command_Length	= scsi->scsi_CmdLength;
	dbs.dbs_Data			= scsi->scsi_Data;
	dbs.dbs_Data_Length		= scsi->scsi_Length;

	stat = MSD_Disk_DoBulk( usbbase, msddisk, & dbs );

//	stat = MSD_Disk_DoBulk( usbbase, msddisk, & dbr, si->cmd, scsi->scsi_CmdLength, scsi->scsi_Data, scsi->scsi_Length, flags );

	scsi->scsi_CmdActual	= scsi->scsi_CmdLength;

	// --

	if (( ! stat ) && ( ! dbr.csw_Valid ))
	{
//		ioreq->io_Actual = 0;
		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	if ( dbr.csw_Valid )
	{
		scsi->scsi_Actual	= dbr.csw_DataActual;
		ioreq->io_Actual	= dbr.csw_DataActual;
	}

	if ( stat )
	{
		// Command passed (Good)
		scsi->scsi_Status	= 0x00;
		goto bailout;
	}

	// Command did not pass. Distinguish CmdFailed vs PhaseError.
	if ( dbr.csw_Status == MSDCSW_Status_CmdFailed )
	{
		scsi->scsi_Status	= 0x02;	// Check Condition

		if (( scsi->scsi_Flags & SCSIF_AUTOSENSE )
		&&	( scsi->scsi_SenseData )
		&&	( scsi->scsi_SenseLength >= 4 ))
		{
			U32 want = MIN( scsi->scsi_SenseLength, 255 );
			want = MAX( want, 4 );

			stat = MSD_SCSI_RequestSense( usbbase, msddisk, & sbr, scsi->scsi_SenseData, want );

			if ( stat )
			{
				scsi->scsi_SenseActual = sbr.csw_DataActual;
			}
		}

		// Transport succeeded (we had a CSW), keep io_Error = 0
		goto bailout;
	}

	// Phase error or other unexpected status -> transport failure
	ioreq->io_Error = IOERR_BADLENGTH;

	// --

bailout:

	TASK_NAME_LEAVE();
}

// --

SEC_CODE S32 MSD_Cmd_001C_HD_SCSICmd( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct __scsi_intern si;
struct SCSICmd *scsi;
S32 reply;

	TASK_NAME_ENTER( "MSD : HD_ScsiCmd" );
	USBERROR( "MSD : _cmd_001C_HD_SCSICmd" );
	USBERROR( "MSD : io_Data .......... : %p", ioreq->io_Data );
	USBERROR( "MSD : io_Length ........ : %ld", ioreq->io_Length );

	reply = TRUE;
	scsi = ioreq->io_Data;

	if (( scsi == NULL )
	||	( ioreq->io_Length < sizeof( struct SCSICmd ))
	||	( scsi->scsi_CmdLength == 0 )
	||	( scsi->scsi_CmdLength > 16 ))
	{
		USBERROR( "MSD : HD_ScsiCmd : bad SCSICmd" );
		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// --

	MEM_SET( & si.cmd, 0, 16 );
	MEM_COPY( scsi->scsi_Command, & si.cmd, scsi->scsi_CmdLength );

	USBERROR( "MSD : scsi_Data ........ : %p",  scsi->scsi_Data );
	USBERROR( "MSD : scsi_Length ...... : %ld", scsi->scsi_Length );
	USBERROR( "MSD : scsi_SenseData ... : %p",  scsi->scsi_SenseData );
	USBERROR( "MSD : scsi_SenseLength . : %ld", scsi->scsi_SenseLength );
	USBERROR( "MSD : scsi_Flags ....... : %ld", scsi->scsi_Flags );
	USBERROR( "MSD : scsi_Command ..... : %p",  scsi->scsi_Command );
	USBERROR( "MSD : scsi_CmdLength ... : %ld", scsi->scsi_CmdLength );
	USBERROR( "MSD : cmd[0] ........... : $%02lx", (U32) si.cmd[0] );
	USBERROR( "MSD : cmd[1] ........... : $%02lx", (U32) si.cmd[1] );
	USBERROR( "MSD : cmd[2] ........... : $%02lx", (U32) si.cmd[2] );
	USBERROR( "MSD : cmd[3] ........... : $%02lx", (U32) si.cmd[3] );
	USBERROR( "MSD : cmd[4] ........... : $%02lx", (U32) si.cmd[4] );
	USBERROR( "MSD : cmd[5] ........... : $%02lx", (U32) si.cmd[5] );

	switch( si.cmd[0] )
	{
		case /* 25 */ SCSI_DA_READ_CAPACITY:
		{
			__HDSCSI_ReadCap10( usbbase, ioreq, & si );
			break;
		}

		default:
		{
			__HDSCSI_Default( usbbase, ioreq, & si );
			break;
		}
	}

bailout:

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
