
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

SEC_CODE enum PhaseStat MSD_Bulk_Cmd_Status( struct USBBase *usbbase, struct MSDDisk *msddisk )
{
struct USB2_EPResource *epr;
struct USB2_IORequest *ioreq;
struct MSDDevice *msddev;
struct ExecIFace *IExec;
//struct MSDDisk *msddisk;
enum PhaseStat stat;
S32 stalled;
S32 tries;

	TASK_NAME_ENTER( "MSD_Bulk_Cmd_Status" );

	msddev	= msddisk->msddisk_MSDDev;
//	msddisk	= msddisk->msdunit_MSDDisk;

//	USBERROR( "MSD : MSD_Disk_DoBulk : MSD_Bulk_Cmd_Status : Lun %ld : Cmd %p : CmdLen %ld : Mem %p : MemLen %ld : Flags $%04lx", msddisk->msddisk_Lun, cmd, cmdlen, mem, memlen, Flags );

	stalled	= FALSE;
	stat	= PS_Error;
	epr		= msddev->msddev_Res_Bulk_In;
	ioreq	= epr->IORequests[0];
	IExec	= usbbase->usb_IExec;

	for( tries=0 ; tries<2 ; tries++ )
	{
		#if defined( DO_ERROR ) || defined( DO_INFO ) || defined( DO_DEBUG )
		MEM_SET( & msddisk->msddisk_CSW, 0xdc, sizeof( struct MSDCSW ));
		#endif

		ioreq->io_Command		= CMD_READ;
		ioreq->io_Data			= & msddisk->msddisk_CSW;
		ioreq->io_Length		= sizeof( struct MSDCSW );
		ioreq->io_SetupData		= NULL;
		ioreq->io_SetupLength	= 0;
		ioreq->io_TimeOut		= 500000;		// 0.5 sec

		IO_DO( ioreq );

//		IExec->DebugPrintF( "CSW  : io_Error = %ld : io_Actual %ld\n", ioreq->io_Error, ioreq->io_Actual );

		/**/ if ( ioreq->io_Error == USB2Err_NoError )
		{
			// I hard coded the Actual value too validate 'struct MSDCSW' size
			if ( ioreq->io_Actual == MSDCSW_LEN )
			{
				stat = ( stalled ) ? PS_Stall : PS_Okay ;
			}
			else
			{
				// Short/long read without STALL -> invalid CSW, fail fast.
				IExec->DebugPrintF ("CSW : unexpected size %ld (wanted %ld)\n", ioreq->io_Actual, (S32) sizeof( struct MSDCSW ));
				stat = PS_Error;
			}
			break;
		}
		else if ( ioreq->io_Error == USB2Err_Device_Detached )
		{
			USBERROR( "MSD_Bulk_Cmd_Status : Detached" );
			MSD_Disk_Detach( usbbase, msddisk );
			stat = PS_Error;
			break;
		}
		else if ( ioreq->io_Error == USB2Err_Host_Stall )
		{
			ENDPOINTRES_DESTALL( epr );
			stalled = TRUE;
			continue;
		}
		else
		{
			// Any other transport error (timeout, CRC, etc.) -> fail
			stat = PS_Error;
			break;
		}
	}

	#if 0
	// -- Lets dump struct
	U32 __pos = 0;
	U8 *__data = (PTR) & msddisk->msddisk_CSW;
	IExec->DebugPrintF( "\n\n### CSW Dump (Status) ###\n" );
	IExec->DebugPrintF( "CSW: Signature: $%02lx %02lx %02lx %02lx\n", __data[__pos+0], __data[__pos+1], __data[__pos+2], __data[__pos+3] );
	__pos += 4;
	IExec->DebugPrintF( "CSW: Tag      : $%02lx %02lx %02lx %02lx\n", __data[__pos+0], __data[__pos+1], __data[__pos+2], __data[__pos+3] );
	__pos += 4;
	IExec->DebugPrintF( "CSW: DResidue : $%02lx %02lx %02lx %02lx\n", __data[__pos+0], __data[__pos+1], __data[__pos+2], __data[__pos+3] );
	__pos += 4;
	IExec->DebugPrintF( "CSW: Status   : $%02lx\n", __data[__pos] );
	__pos += 1;
	IExec->DebugPrintF( "__pos %ld\n", __pos );
	#endif

	TASK_NAME_LEAVE();
	return( stat );
}

// --
