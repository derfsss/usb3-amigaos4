
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

SEC_CODE enum PhaseStat MSD_Bulk_Cmd_Phase( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkStruct *dbs )
{
struct USB2_EPResource *epr;
struct USB2_IORequest *ioreq;
struct MSDDevice *msddev;
//struct ExecIFace *IExec;
//struct MSDDisk *msddisk;
enum PhaseStat stat;
U32 IsRead;

	TASK_NAME_ENTER( "MSD_Bulk_Cmd_Phase" );

	msddev	= msddisk->msddisk_MSDDev;
//	msddisk	= msddisk->msdunit_MSDDisk;

//	USBERROR( "MSD : MSD_Disk_DoBulk : MSD_Bulk_Cmd_Phase : Lun %ld : Cmd %p : CmdLen %ld : Mem %p : MemLen %ld : Flags $%04lx", msddisk->msddisk_Lun, cmd, cmdlen, mem, memlen, Flags );

	#if defined( DO_ERROR ) || defined( DO_INFO ) || defined( DO_DEBUG )
	MEM_SET( & msddisk->msddisk_CBW, 0xac, sizeof( struct MSDCBW ));
	#endif

	epr		= msddev->msddev_Res_Bulk_Out;
	ioreq	= epr->IORequests[0];
//	IExec	= usbbase->usb_IExec;

	// Direction bit only matters if a data phase exists
	IsRead	= (( dbs->dbs_Flags & DOBULKF_DIR_MASK ) == DOBULKF_DIR_IN );

	// -- Init cbw struct
	msddisk->msddisk_CBW.cbw_Signature		= LE_SWAP32( MSDCBW_Signature );
	msddisk->msddisk_CBW.cbw_Tag			= LE_SWAP32( msddev->msddev_Bulk_TagID );
	msddisk->msddisk_CBW.cbw_DataLength		= LE_SWAP32( dbs->dbs_Data_Length );
	// Don't set DataIn flag if there are no Mem/MemLen and we are a Write command
	msddisk->msddisk_CBW.cbw_Flags			= (( IsRead ) && ( dbs->dbs_Data ) && ( dbs->dbs_Data_Length )) ? MSDCBW_Flags_DataIn : 0 ;
	msddisk->msddisk_CBW.cbw_LUN			= ( msddisk->msddisk_Lun & 0x0F );
	msddisk->msddisk_CBW.cbw_CBLength		= dbs->dbs_Command_Length;

	// The cmd my only be eg. 6 bytes, but sender
	// have to send a 16 bytes zero padded buffer
	MEM_COPY( dbs->cmd, & msddisk->msddisk_CBW.cbw_CB[0], 16 );

	#if 0
	// -- Lets dump struct
	U32 __pos = 0;
	U8 *__data = (PTR) & msddisk->msddisk_CBW ;
	struct ExecIFace *IExec = usbbase->usb_IExec;
	IExec->DebugPrintF( "\n\n### 1 ###\n" );
	IExec->DebugPrintF( "CBW: Signature: $%02lx %02lx %02lx %02lx\n", __data[__pos+0], __data[__pos+1], __data[__pos+2], __data[__pos+3] );
	__pos += 4;
	IExec->DebugPrintF( "CBW: Tag      : $%02lx %02lx %02lx %02lx\n", __data[__pos+0], __data[__pos+1], __data[__pos+2], __data[__pos+3] );
	__pos += 4;
	IExec->DebugPrintF( "CBW: DataLen  : $%02lx %02lx %02lx %02lx\n", __data[__pos+0], __data[__pos+1], __data[__pos+2], __data[__pos+3] );
	__pos += 4;
	IExec->DebugPrintF( "CBW: Flags    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: LUN      : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CBLen    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[0]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[1]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[2]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[3]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[4]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[5]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[6]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[7]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[8]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[9]    : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[10]   : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[11]   : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[12]   : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[13]   : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[14]   : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "CBW: CB[15]   : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "__pos %ld\n", __pos );
	#endif

	// -- Send Command Block Wrapper
	ioreq->io_Command		= CMD_WRITE;
	ioreq->io_Data			= & msddisk->msddisk_CBW;
	ioreq->io_Length		= sizeof( struct MSDCBW );
	ioreq->io_SetupData		= NULL;
	ioreq->io_SetupLength	= 0;
	ioreq->io_TimeOut		= 500000;		// 0.5 sec

	IO_DO( ioreq );

//	IExec->DebugPrintF( "CBW  : io_Error = %ld : io_Actual %ld\n", ioreq->io_Error, ioreq->io_Actual );

	// I hard coded the Actual value too validate 'struct MSDCBW' size
	/**/ if (( ioreq->io_Error == USB2Err_NoError ) && ( ioreq->io_Actual == MSDCBW_LEN ))
	{
		// Main loop will do a Recovery for us
		stat = PS_Okay;
	}
	else if ( ioreq->io_Error == USB2Err_Device_Detached )
	{
		USBERROR( "MSD_Bulk_Cmd_Phase : Detached" );
		MSD_Disk_Detach( usbbase, msddisk );
		stat = PS_Error;
	}
	else
	{
		if ( ioreq->io_Error == USB2Err_Host_Stall )
		{
			ENDPOINTRES_DESTALL( epr );
		}

		stat = PS_Error;
	}

	TASK_NAME_LEAVE();
	return( stat );
}

// --
