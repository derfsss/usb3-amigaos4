
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

SEC_RODATA static const struct TagItem memtags[] =
{
	{ AVT_Type, MEMF_SHARED },
	{ TAG_END, TAG_END }
};

SEC_CODE S32 MSD_Cmd_A004_NSCmd_GetGeometry64( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct DriveGeometry64 tmp_dg;
struct MSDDisk *msddisk;
U32 actual;
S32 reply;
U32 flags;

	TASK_NAME_ENTER( "MSD : NSCmd_GetGeometry64" );
	USBERROR( "MSD : _cmd_A004_NSCmd_GetGeometry64" );

	reply = TRUE;

	if (( ! ioreq->io_Data ) || ( ! ioreq->io_Length ))
	{
		ioreq->io_Error = IOERR_BADLENGTH;
		USBERROR( "MSD : NULL Pointer" );
		goto bailout;
	}

	msddisk = (PTR) ioreq->io_Unit;

	MEM_SET( & tmp_dg, 0, sizeof( struct DriveGeometry64 ));

	flags = 0;

	if ( msddisk->msddisk_Removable )
	{
		flags |= DGF_REMOVABLE;
	}

	tmp_dg.dg_SectorSize	= msddisk->msddisk_Block_Size;
//	tmp_dg.dg_Reserved1		= 0;
	tmp_dg.dg_TotalSectors	= msddisk->msddisk_TotalBlocks;
	tmp_dg.dg_BufMemTags	= memtags;
	tmp_dg.dg_DeviceType	= msddisk->msddisk_DeviceType;
	tmp_dg.dg_Flags			= flags;
//	tmp_dg.dg_Reserved2		= 0;

	actual = MIN( ioreq->io_Length, sizeof( struct DriveGeometry64 ));

	// Update User Buffers
	MEM_COPY( & tmp_dg, ioreq->io_Data, actual );
	ioreq->io_Actual = actual;
	ioreq->io_Error = 0;

	#if 0
	struct ExecIFace *IExec = usbbase->usb_IExec;
	IExec->DebugPrintF( "32 dg_SectorSize ... : %lu\n",  tmp_dg.dg_SectorSize );
	IExec->DebugPrintF( "64 dg_Reserved1 .... : %llu\n", tmp_dg.dg_Reserved1 );
	IExec->DebugPrintF( "64 dg_TotalSectors . : %llu\n", tmp_dg.dg_TotalSectors );
	IExec->DebugPrintF( "32 dg_BufMemTags ... : %p\n",	 tmp_dg.dg_BufMemTags );
	IExec->DebugPrintF( " 8 dg_DeviceType  .. : %lu\n",  tmp_dg.dg_DeviceType );
	IExec->DebugPrintF( " 8 dg_Flags ........ : %lu\n",  tmp_dg.dg_Flags );
	IExec->DebugPrintF( "16 dg_Reserved2 .... : %lu\n",  tmp_dg.dg_Reserved2 );
	#endif

bailout:

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
