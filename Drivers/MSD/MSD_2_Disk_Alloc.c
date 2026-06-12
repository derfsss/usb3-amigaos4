
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

SEC_CODE struct MSDDisk *MSD_Disk_Alloc( struct USBBase *usbbase, struct MSDDevice *msddev, U32 lun )
{
struct MSDBase *msdbase;
struct MSDDisk *msddisk;
U32 capdata10[2];
U32 blk_size;
U32 unit_nr;
S32 error;
S32 stat;
U32 cnt;
U8 inquiry[36];
U8 type;

	TASK_NAME_ENTER( "MSD_Disk_Alloc" );
	USBERROR( "MSD : MSD_Disk_Alloc" );

	// ---

	error = TRUE;

	msdbase = usbbase->usb_MSDDevice;
	msddisk = MEM_ALLOCVEC( sizeof( struct MSDDisk ), TRUE );

	if ( ! msddisk )
	{
		USBERROR( "MSD : MSD_Disk_Alloc : Error allocating memory" );
		goto bailout;
	}

	msddisk->msddisk_Lun = lun;
	msddisk->msddisk_MSDDev = msddev;
	msddisk->msddisk_MSDBase = msdbase;
	msddisk->msddisk_GeoChangeCount = -1;

	// --
	// Inquire MSD Type

	for( cnt=0 ; cnt<3 ; cnt++ )
	{
		USBERROR( "MSD : MSD_Disk_Alloc : MSD_SCSI_Inquiry : Lun #%lu : Cnt #%lu", lun, cnt );

		if ( MSD_SCSI_Inquiry( usbbase, msddisk, inquiry, 36 ))
		{
			USBERROR( "MSD : MSD_Disk_Alloc : MSD_SCSI_Inquiry : #%ld : Okay", cnt );
			break;
		}
		else
		{
			USBERROR( "MSD : MSD_Disk_Alloc : MSD_SCSI_Inquiry : Try #%ld/3 : Failed", cnt+1 );

			MISC_MICRODELAY( 100 * 1000 );		// Wait 100ms
		}
	}

	if ( cnt >= 3 )
	{
		// No responce, proberly nobody there
		USBERROR( "MSD : MSD_Disk_Alloc : No reponce for Lun #%lu", lun );
		goto bailout;
	}

	// --
	// Check MSD Type

	type = inquiry[0] & 0x1F;

	/**/ if ( type == DG_DIRECT_ACCESS )
	{
		USBERROR( "MSD : MSD_Disk_Alloc : Found new 'Direct Access' device" );
	}
	else if ( type == DG_OPTICAL_DISK )
	{
		USBERROR( "MSD : MSD_Disk_Alloc : Found new 'Optical Disk' device" );
	}
	else if ( type == DG_CDROM )
	{
		USBERROR( "MSD : MSD_Disk_Alloc : Found new 'CDRom' device" );
	}
	else
	{
		USBERROR( "MSD : MSD_Disk_Alloc : Unsupported Device Type (%lu)", type );
		goto bailout;
	}

	msddisk->msddisk_DeviceType	= type;
	msddisk->msddisk_Removable	= ( inquiry[1] & 0x80 ) ? TRUE : FALSE;

	// --
	// We need to execute a Test Unit Ready .. too turn on the device

	if ( ! MSD_Disk_TestUnitReady( usbbase, msddisk ))
	{
		USBERROR( "MSD : MSD_Disk_Alloc : MSD_Disk_TestUnitReady() failed" );
		goto bailout;
	}

	// --
	// Get Block Size

	stat = MSD_SCSI_ReadCapacity10( usbbase, msddisk, NULL, capdata10, sizeof( capdata10 ));

	if ( ! stat )
	{
		USBERROR( "MSD : MSD_Disk_Parse : MSD_SCSI_ReadCapacity10() failed" );
		goto bailout;
	}

	// --
	// Sanity on block size

	blk_size = BE_SWAP32( capdata10[1] );

	if (( ! blk_size ) || (( blk_size & ( blk_size - 1 )))) 
	{
		// Not a power of two -> fail (or fallback to 512 if you prefer)
		USBERROR( "MSD : TD_GetGeometry : bad Block Size %llu (Defaults to 512)", blk_size );
		blk_size = 512;
	}

	// Mask and Shift is too avoid 64bit compiler __umoddi3 and __udivi3 functions
	msddisk->msddisk_TotalBlocks = BE_SWAP32( capdata10[0] );
	msddisk->msddisk_Block_Size = blk_size;
	msddisk->msddisk_Block_Mask = blk_size - 1;
	msddisk->msddisk_Block_Shift = 0;

	while( blk_size > 1 )
	{
		msddisk->msddisk_Block_Shift++;
		blk_size >>= 1;
	}

	USBERROR( "msddisk_Block_Size ... : %lu",  msddisk->msddisk_Block_Size );
	USBERROR( "msddisk_Block_Mask ... : %lu",  msddisk->msddisk_Block_Mask );
	USBERROR( "msddisk_Block_Shift .. : %lu",  msddisk->msddisk_Block_Shift );
	USBERROR( "msddisk_TotalBlocks .. : %llu", msddisk->msddisk_TotalBlocks );

	// --
	// Mount

	msddisk->msddisk_MsgPort_Abort = & msddev->msddev_MsgPort_Abort;
	msddisk->msddisk_MsgPort_Begin = & msddev->msddev_MsgPort_Begin;

	// --
	// End Init with, find Unit slot and then Add/Mount

	SEMAPHORE_OBTAIN( & msdbase->msdbase_MSDDisk_Semaphore );

	for( unit_nr=0 ; unit_nr<MAX_MSD_UNITS ; unit_nr++ )
	{
		if ( ! msdbase->msdbase_MSDDisk_Units[ unit_nr ] )
		{
			msdbase->msdbase_MSDDisk_Units[ unit_nr ] = msddisk;
			break;
		}
	}

	SEMAPHORE_RELEASE( & msdbase->msdbase_MSDDisk_Semaphore );

	if ( unit_nr >= MAX_MSD_UNITS )
	{
		USBERROR( "MSD : MSD_Unit_Alloc : No free Unit slots" );
		goto bailout;
	}

	// When we set UnitNr, it also means we have added it to Header
	msddisk->msddisk_UnitNr = unit_nr;

	// --

	stat = usbbase->usb_IMounter->AnnounceDeviceTags( MSD_Device_Name, msddisk->msddisk_UnitNr,
		MNTA_DosNamePrefixHint, "USB",
		TAG_END
	);

	msddisk->msddisk_Announced = ( stat ) ? TRUE : FALSE ;
	USBERROR( "Mount Stat: %ld (%s/%ld)", stat, "USB", msddisk->msddisk_UnitNr );

	// --

	error = FALSE;

	USBERROR( "hmm disk okay" );

bailout:

	if (( msddisk ) && ( error ))
	{
		MSD_Disk_Free( usbbase, msddisk );
		msddisk = NULL;
	}

	TASK_NAME_LEAVE();
	return( msddisk );
}

// --
