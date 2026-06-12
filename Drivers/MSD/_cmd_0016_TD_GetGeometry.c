
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

SEC_CODE S32 MSD_Cmd_0016_TD_GetGeometry( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct DriveGeometry tmp_dg;
struct MSDDisk *msddisk;
U32 capdata10[2];
U64 capdata16[2];
U32 blk_size;	// Block size 512/2048
U32 tmp_size;
U64 blk_last;	// Last Block nr
U64 blk_secs;	// Total Sectors
U32 actual;
S32 reply;

	TASK_NAME_ENTER( "MSD : TD_GetGeometry" );
	USBERROR( "MSD : _cmd_0016_TD_GetGeometry" );

	msddisk = (PTR) ioreq->io_Unit;


	reply = TRUE;

	if (( ! ioreq->io_Data ) || ( ! ioreq->io_Length ))
	{
		ioreq->io_Error = IOERR_BADLENGTH;
		USBERROR( "MSD : NULL Pointer" );
		goto bailout;
	}

	actual = MIN( ioreq->io_Length, sizeof( struct DriveGeometry ));

	#if 0
	// If unchanged, return cached geometry
	if ( msddisk->msddisk_GeoChangeCount == msddisk->msddisk_DiskChangeCount )
	{
		USBERROR( "MSD : No change" );
		MEM_COPY( & msddisk->msddisk_Geometry, ioreq->io_Data, actual );
		ioreq->io_Actual = actual;
		ioreq->io_Error = 0;
		goto bailout;
	}
	#endif

	MEM_SET( & tmp_dg, 0, sizeof( struct DriveGeometry ));

	if ( ! MSD_SCSI_ReadCapacity10( usbbase, msddisk, NULL, capdata10, sizeof( capdata10 )))
	{
		USBERROR( "MSD : MSD_SCSI_ReadCapacity10() failed" );
		ioreq->io_Error = IOERR_BADLENGTH;
		goto bailout;
	}

	// If RC10 says 0xFFFFFFFF, do RC16 (0x9E/0x10) for 32 bytes 
	if ( capdata10[0] == 0xFFFFFFFFUL )
	{
		// todo make ReadCap16
//		if ( ! MSD_SCSI_ReadCapacity16( usbbase, msddisk, NULL, capdata16, sizeof( capdata16 )))
		{
			USBERROR( "MSD : MSD_SCSI_ReadCapacity16() failed" );
			ioreq->io_Error = IOERR_BADLENGTH;
			goto bailout;
		}

		blk_last	= BE_SWAP64( capdata16[0] );
		blk_size	= BE_SWAP64( capdata16[1] );	// fix me
		blk_secs	= blk_last + 1;
	}
	else
	{
		blk_last	= BE_SWAP32( capdata10[0] );
		blk_size 	= BE_SWAP32( capdata10[1] );
		blk_secs	= blk_last + 1;
	}

	// todo : 
	// Make sure blksz is non-zero and reasonable (often 512/1024/2048/4096 and power-of-two). If it is weird, default to 512 or fail gracefully.

	// Sanity on block size
	if (( ! blk_size ) || (( blk_size & ( blk_size - 1 )))) 
	{
		// Not a power of two -> fail (or fallback to 512 if you prefer)
		USBERROR( "MSD : TD_GetGeometry : bad Block Size %llu (Defaults to 512)", blk_size );
		blk_size = 512;
	}

	// tmp setting, as I do no calc.. 
	// proberly only good for small storages devices 
	tmp_dg.dg_SectorSize	= (U32) blk_size;
	tmp_dg.dg_TotalSectors	= (U32) blk_secs;
	tmp_dg.dg_Cylinders		= (U32) blk_secs;
	tmp_dg.dg_CylSectors	= 1;
	tmp_dg.dg_Heads			= 1;
	tmp_dg.dg_TrackSectors	= 1;
	tmp_dg.dg_BufMemType	= MEMF_PUBLIC;
	tmp_dg.dg_DeviceType	= msddisk->msddisk_DeviceType;
	tmp_dg.dg_Flags			= ( msddisk->msddisk_Removable ) ? DGF_REMOVABLE : 0 ;
	tmp_dg.dg_Reserved		= 0;

	// We are okay now

	// Update Internal Buffers
	MEM_COPY( & tmp_dg, & msddisk->msddisk_Geometry, sizeof( struct DriveGeometry ));
	msddisk->msddisk_GeoChangeCount = msddisk->msddisk_DiskChangeCount;
//	msddisk->msddisk_Block_Size = (U32) blk_size;
	msddisk->msddisk_TotalBlocks = blk_secs;

	// Mask and Shift is too avoid 64bit compiler __umoddi3 and __udivi3 functions
	msddisk->msddisk_Block_Size = blk_size;
	msddisk->msddisk_Block_Mask = blk_size - 1;
	msddisk->msddisk_Block_Shift = 0;

	tmp_size = blk_size;
	while( tmp_size > 1 )
	{
		msddisk->msddisk_Block_Shift++;
		tmp_size >>= 1;
	}

	// --
	// Update User Buffers
	MEM_COPY( & msddisk->msddisk_Geometry, ioreq->io_Data, actual );
	ioreq->io_Actual = actual;
	ioreq->io_Error = 0;

	#if 1
	struct ExecIFace *IExec = usbbase->usb_IExec;
	IExec->DebugPrintF( "64 blk_last ........ : %llu\n", blk_last );
	IExec->DebugPrintF( "32 blk_size ........ : %lu\n",  blk_size );
	IExec->DebugPrintF( "32 blk_mask ........ : %lu\n",  msddisk->msddisk_Block_Mask );
	IExec->DebugPrintF( "32 blk_shift ....... : %lu\n",  msddisk->msddisk_Block_Shift );
	IExec->DebugPrintF( "64 blk_secs ........ : %llu\n", blk_secs );
	IExec->DebugPrintF( "32 dg_SectorSize ... : %lu\n", msddisk->msddisk_Geometry.dg_SectorSize );
	IExec->DebugPrintF( "32 dg_TotalSectors . : %lu\n", msddisk->msddisk_Geometry.dg_TotalSectors );
	IExec->DebugPrintF( "32 dg_Cylinders .... : %lu\n", msddisk->msddisk_Geometry.dg_Cylinders );
	IExec->DebugPrintF( "32 dg_CylSectors ... : %lu\n", msddisk->msddisk_Geometry.dg_CylSectors );
	IExec->DebugPrintF( "32 dg_Heads  ....... : %lu\n", msddisk->msddisk_Geometry.dg_Heads );
	IExec->DebugPrintF( "32 dg_TrackSectors . : %lu\n", msddisk->msddisk_Geometry.dg_TrackSectors );
	IExec->DebugPrintF( "32 dg_BufMemType ... : %lu\n", msddisk->msddisk_Geometry.dg_BufMemType );
	IExec->DebugPrintF( " 8 dg_DeviceType ... : %u\n",  msddisk->msddisk_Geometry.dg_DeviceType );
	IExec->DebugPrintF( " 8 dg_Flags ........ : %u\n",  msddisk->msddisk_Geometry.dg_Flags );
	IExec->DebugPrintF( "16 dg_Reserved ..... : %lu\n", msddisk->msddisk_Geometry.dg_Reserved );
	#endif

bailout:

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
