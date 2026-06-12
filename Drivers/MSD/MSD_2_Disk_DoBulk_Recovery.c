
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

SEC_CODE void MSD_DoBulk_Recovery( struct USBBase *usbbase, struct MSDDisk *msddisk )
{
struct USB2_IORequest *ioreq;
struct USB2_SetupData *sd;
//struct DoBulkResult dbr;
struct MSDDevice *msddev;
//struct MSDDisk *msddisk;
struct ExecIFace *IExec;
//U32 oldtime;
//U8 sense[18];

	TASK_NAME_ENTER( "MSD_DoBulk_Recovery" );
	USBERROR( "MSD : MSD_DoBulk_Recovery" );

	IExec = usbbase->usb_IExec;

//
	msddev	= msddisk->msddisk_MSDDev;

	// --
	// Check if we allready are in Recovery Mode

	if ( msddisk->msddisk_InRecovery )
	{
		USBERROR( "MSD : MSD_DoBulk_Recovery : Allready in Recovery" );
		goto bailout;
	}

	msddisk->msddisk_InRecovery = TRUE;

	// -- Send Recovery

	IExec->DebugPrintF( "MSD_DoBulk_Recovery 11\n" );

	sd = msddev->msddev_Res_Control->SetupData;
	sd->RequestType		= REQTYPE_Write | REQTYPE_Class | REQTYPE_Interface;
	sd->RequestCode		= REQCODE_MassStorage_Reset;
	sd->Value			= LE_SWAP16( 0 );
	sd->Index			= LE_SWAP16( msddev->msddev_InterfaceNr );
	sd->Length			= LE_SWAP16( 0 );

	ioreq = msddev->msddev_Res_Control->IORequests[0];
	ioreq->io_Command		= CMD_WRITE;
	ioreq->io_Data			= NULL;
	ioreq->io_Length		= 0;
	ioreq->io_SetupData		= sd;
	ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

//	oldtime = ioreq->io_TimeOut;
//	ioreq->io_TimeOut = 500000;  // 0.5s

	IO_DO( ioreq );

//	ioreq->io_TimeOut = oldtime;

	IExec->DebugPrintF( "MSD_DoBulk_Recovery 12 : io_Error %ld\n", (S32) ioreq->io_Error );

	// Settle delay
	MISC_MICRODELAY( 10 * 1000 );		// wait 10ms

	// Send Clear Stall on Bulk

	IExec->DebugPrintF( "MSD_DoBulk_Recovery 13\n" );

	ENDPOINTRES_DESTALL( msddev->msddev_Res_Bulk_In );

	IExec->DebugPrintF( "MSD_DoBulk_Recovery 14\n" );

	ENDPOINTRES_DESTALL( msddev->msddev_Res_Bulk_Out );

	IExec->DebugPrintF( "MSD_DoBulk_Recovery 15\n" );

	// Optional tiny breather after de-stalls
	MISC_MICRODELAY( 5 * 1000 );		// wait 5ms

	// --
	#if 0

	MEM_SET( & dbr, 0x33, sizeof( dbr ));
	MEM_SET( & sense, 0x55, sizeof( sense ));

	IExec->DebugPrintF( "MSD_DoBulk_Recovery 16\n" );

	if ( ! MSD_SCSI_TestUnitReady( usbbase, msddisk, & dbr ))
	{
		IExec->DebugPrintF( "MSD_DoBulk_Recovery 17\n" );

		// TUR failed -> ask for sense (fixed format, 18 bytes)
		if ( MSD_SCSI_RequestSense( usbbase, msddisk, NULL, sense, sizeof( sense ))) 
		{
			U8 key  = sense[2] & 0x0F;
			U8 asc  = sense[12];
			U8 ascq = sense[13];

			IExec->DebugPrintF( "Recovery/TUR: Sense key=%02lx ASC=%02lx ASCQ=%02lx\n", key, asc, ascq );

			// No fallback here (per your plan). Just good logging:
			// if (key==0x05 && asc==0x20 && ascq==0x00) -> device does not support READ(16).
		} 
		else 
		{
			IExec->DebugPrintF( "Recovery/TUR: RequestSense failed\n" );
		}
	} 
	else 
	{
		IExec->DebugPrintF( "Recovery/TUR: Unit is ready\n ");
	}

	IExec->DebugPrintF( "MSD_DoBulk_Recovery 18\n" );

	#endif
	// --
	msddisk->msddisk_InRecovery = FALSE;

bailout:

	// --
	TASK_NAME_LEAVE();
}

// --
