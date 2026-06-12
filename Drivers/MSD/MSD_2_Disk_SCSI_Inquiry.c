
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

SEC_CODE S32 MSD_SCSI_Inquiry( struct USBBase *usbbase, struct MSDDisk *msddisk, PTR mem, U32 len )
{
struct DoBulkStruct dbs;
S32 retval;

	TASK_NAME_ENTER( "MSD_SCSI_Inquiry" );
	USBERROR( "MSD : MSD_SCSI_Inquiry" );

	retval = FALSE;

	// --
	MEM_SET( & dbs, 0, sizeof( dbs ));
	dbs.dbs_Flags			= DOBULKF_DIR_IN | DOBULKF_SHORT_OK;
	dbs.dbs_Command_Length	= 6;
	dbs.dbs_Data			= mem;
	dbs.dbs_Data_Length		= len;
	dbs.cmd[ 0]	= SCSI_INQUIRY; 	// (0x12)
	dbs.cmd[ 1]	= 0;				// EVPD bit + reserved
	dbs.cmd[ 2]	= 0;				// Page code (not used if EVPD=0)
	dbs.cmd[ 3]	= ( len >>  8 );	// Allocation length
	dbs.cmd[ 4]	= ( len >>  0 );	// Allocation length
	dbs.cmd[ 5]	= 0;				// Control byte
	dbs.cmd[ 6]	= 0;
	dbs.cmd[ 7]	= 0;
	dbs.cmd[ 8]	= 0;
	dbs.cmd[ 9]	= 0;
	dbs.cmd[10]	= 0;
	dbs.cmd[11]	= 0;
	dbs.cmd[12]	= 0;
	dbs.cmd[13]	= 0;
	dbs.cmd[14]	= 0;
	dbs.cmd[15]	= 0;

	// -- Execute
	if ( ! MSD_Disk_DoBulk( usbbase, msddisk, & dbs ))
	{
		USBERROR( "MSD_SCSI_Inquiry() failed" );
		goto bailout;
	}

	#if 0

{
//U8 inquiry[45];

//	MEM_COPY( mem, inquiry, 44 );

	// 36-44 is optional
//	inquiry[44] = 0;
//	usbbase->usb_IExec->DebugPrintF( "Serial.......: '%s'\n", & inquiry[36] );
//	inquiry[36] = 0;

	U8 devType = ((U8*)mem)[0] & 0x1F;
	U8 version = ((U8*)mem)[2];
	U8 addlen  = ((U8*)mem)[4];

	usbbase->usb_IExec->DebugPrintF( "Device Type..: $%02lx\n", devType);
	usbbase->usb_IExec->DebugPrintF( "Version......: %ld\n", version);
	usbbase->usb_IExec->DebugPrintF( "Add. Length..: %ld\n", addlen);
	usbbase->usb_IExec->DebugPrintF( "Vendor.......: '%.8s'\n",  (char *)mem + 8);
	usbbase->usb_IExec->DebugPrintF( "Product......: '%.16s'\n", (char *)mem + 16);
	usbbase->usb_IExec->DebugPrintF( "Revision.....: '%.4s'\n",  (char *)mem + 32);
}
	#endif

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();
	return( retval );
}

// --
