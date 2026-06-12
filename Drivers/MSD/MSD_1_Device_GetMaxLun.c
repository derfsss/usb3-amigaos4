
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

SEC_CODE U32 MSD_Device_GetMaxLun( struct USBBase *usbbase, struct MSDDevice *msddev )
{
struct USB2_IORequest *ioreq;
struct USB2_SetupData *sd;
U32 maxlun;
U32 cnt;
U8 data[1];

	TASK_NAME_ENTER( "MSD_GetMaxLun" );
	USBERROR( "MSD : MSD_GetMaxLun" );

	ioreq	= msddev->msddev_Res_Control->IORequests[0];
	sd		= msddev->msddev_Res_Control->SetupData;

	maxlun	= 0;

	for( cnt=0 ; cnt<3 ; cnt++ )
	{
		sd->RequestType			= REQTYPE_Read | REQTYPE_Class | REQTYPE_Interface;
		sd->RequestCode			= REQCODE_MassStorage_GetMaxLUN;
		sd->Value				= LE_SWAP16( 0 );
		sd->Index				= LE_SWAP16( msddev->msddev_InterfaceNr );
		sd->Length				= LE_SWAP16( 1 );

		ioreq->io_Command		= CMD_READ;
		ioreq->io_Data			= & data;
		ioreq->io_Length		= 1;
		ioreq->io_SetupData		= sd;
		ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

		IO_DO( ioreq );

		/**/ if ( ioreq->io_Error == USB2Err_NoError )
		{
			maxlun = data[0];
			
			USBERROR( "MSD_GetMaxLun : Got MaxLuns (%ld)", maxlun );
			break;
		}
		else if ( ioreq->io_Error == USB2Err_Host_Stall )
		{
			break;
		}
		else if ( ioreq->io_Error == USB2Err_Device_Detached )
		{
			USBERROR( "MSD_Device_GetMaxLun : Detached" );
			msddev->msddev_Detached = TRUE;
			break;
		}

		// do 500ms delay here
		MISC_MICRODELAY( 500 * 1000 );
	}

	if ( cnt == 3 )
	{
		USBERROR( "MSD_GetMaxLun : Error retriving MSD MaxLun" );
	}

	if ( maxlun > 15 )
	{
		maxlun = 15;
	}

	TASK_NAME_LEAVE();
	return(	maxlun );
}

// --
