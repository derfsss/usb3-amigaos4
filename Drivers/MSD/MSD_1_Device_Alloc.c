
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

SEC_CODE struct MSDDevice *MSD_Device_Alloc( struct USBBase *usbbase, struct USB2_DriverMessage *msg )
{
struct USB2_Interface_Desc *ifcdsc;
struct RealRegister *reg;
struct MSDDevice *msddev;
struct MSDDisk *msddisk;
U32 do_bulk_out;
U32 do_bulk_in;
U32 MaxLun;
U32 ifcnr;
S32 error;
U32 lun;

	TASK_NAME_ENTER( "MSD_Device_Alloc" );
	USBERROR( "MSD : MSD_Device_Alloc" );

	// ---

	error = TRUE;

	msddev = MEM_ALLOCVEC( sizeof( struct MSDDevice ), TRUE );

	if ( ! msddev )
	{
		USBERROR( "MSD : MSD_Device_Alloc : Error allocating memory" );
		goto bailout;
	}

	msddev->msddev_StartMsg = msg;
	msddev->msddev_MSDBase = usbbase->usb_MSDDevice;

	// ---
	// AbortIO MsgPort

	if ( ! MSGPORT_INIT( & msddev->msddev_MsgPort_Abort ))
	{
		USBDEBUG( "MSD : MSD_Device_Alloc : Abort MsgPort init failed" );
		goto bailout;
	}

	// BeginIO MsgPort

	if ( ! MSGPORT_INIT( & msddev->msddev_MsgPort_Begin ))
	{
		USBDEBUG( "MSD : MSD_Device_Alloc : Begin MsgPort init failed" );
		goto bailout;
	}

	// ---

	reg = REGISTER_REGISTERTAGS(
		USB2Tag_Reg_DriverMessage, msg,
		USB2Tag_Reg_Title, "MSD",
		TAG_END
	);

	if ( ! reg )
	{
		USBERROR( "MSD : MSD_Device_Alloc : Error Registering Device" );
		goto bailout;
	}

	msddev->msddev_Register = reg;
	msddev->msddev_Res_Control = reg->reg_Public.Res_Control;

	ifcdsc	= msg->InterfaceDescriptor;
	ifcnr	= ifcdsc->InterfaceNumber;

	msddev->msddev_InterfaceNr = ifcnr;

	// ---
	// Support check

	do_bulk_in	= TRUE;
	do_bulk_out	= TRUE;

	switch( ifcdsc->InterfaceSubClass )
	{
		case /* 0x06 */ MSD_SubClass_SCSI:
		{
			switch( ifcdsc->InterfaceProtocol )
			{
				case /* 0x50 */ MSD_Protocol_Bulk:
				{
					do_bulk_in	= TRUE;
					do_bulk_out	= TRUE;
					break;
				}

				default:
				{
					USBERROR( "MSD : MSD_Device_Alloc : Unsupported MSD Protocol (%ld)", ifcdsc->InterfaceProtocol );
					goto bailout;
				}
			}
			break;
		}

		default:
		{
			USBERROR( "MSD : MSD_Device_Alloc : Unsupported MSD Subclass (%ld)", ifcdsc->InterfaceSubClass );
			goto bailout;
		}
	}

	// ---

	if ( do_bulk_in )
	{
		msddev->msddev_Res_Bulk_In = ENDPOINTRES_OBTAINTAGS( reg,
			USB2Tag_EPRes_InterfaceNr, ifcnr,
			USB2Tag_EPRes_EPDirection, EPADR_Dir_In,
			USB2Tag_EPRes_EPType, EPATT_Type_Bulk,
			TAG_END
		);

		if ( ! msddev->msddev_Res_Bulk_In )
		{
			USBERROR( "MSD : MSD_Device_Alloc : Error obtaining EndPoint Resource" );
			goto bailout;
		}
	}

	if ( do_bulk_out )
	{
		msddev->msddev_Res_Bulk_Out = ENDPOINTRES_OBTAINTAGS( reg,
//			USB2Tag_EPRes_AddZeroPacket, TRUE,
			USB2Tag_EPRes_InterfaceNr, ifcnr,
			USB2Tag_EPRes_EPDirection, EPADR_Dir_Out,
			USB2Tag_EPRes_EPType, EPATT_Type_Bulk,
			TAG_END
		);

		if ( ! msddev->msddev_Res_Bulk_Out )
		{
			USBERROR( "MSD : MSD_Device_Alloc : Error obtaining EndPoint Resource" );
			goto bailout;
		}

//		struct RealRequest *ioreq = msddev->msddev_Res_Bulk_Out->IORequests[0];
//		USBERROR( "msddev_Res_Bulk_Out : AddZP %ld", ioreq->req_Public.io_AddZeroPackets );
	}

	// --

	if ( ifcdsc->InterfaceProtocol == MSD_Protocol_Bulk )
	{
		USBERROR( "MSD : MSD_Device_Alloc : Retrieving MSD MaxLun" );

		MaxLun = MSD_Device_GetMaxLun( usbbase, msddev );
	}
	else
	{
		MaxLun = 0; // Zero if not Bulk
	}

	USBERROR( "MSD : MSD_Device_Alloc : MSD MaxLuns %ld", MaxLun );

	for( lun=0 ; lun <= MaxLun ; lun++ )
	{
		msddisk = MSD_Disk_Alloc( usbbase, msddev, lun );

		if ( ! msddisk )
		{
			USBERROR( "MSD : MSD_Device_Alloc : Error Disk Node" );
			goto bailout;
		}

		NODE_ADDTAIL( & msddev->msddev_MSDDisk_Header, & msddisk->msddisk_Node );
	}

	// --

	error = FALSE;

bailout:

	if (( msddev ) && ( error ))
	{
		MSD_Device_Free( usbbase, msddev );
		msddev = NULL;
	}

	TASK_NAME_LEAVE();
	return( msddev );
}

// --
