
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

SEC_CODE void MSD_Main__myHandle_Begin( struct USBBase *usbbase, struct MSDDevice *msddev )
{
struct IOStdReq *ioreq;
S32 reply;

	TASK_NAME_ENTER( "MSD : __myHandle_Begin" );
//	USBERROR( "MSD : __myHandle_Begin" );

	while( TRUE )
	{
		ioreq = MSGPORT_GETMSG( & msddev->msddev_MsgPort_Begin );

		if ( ! ioreq )
		{
			// No more IORequests.. not an error
			break;
		}

		// Device gone: fail data commands immediately instead of
		// timing out against dead hardware. Status commands and the
		// ones filesystems need for an orderly teardown still work.
		if ( msddev->msddev_Detached )
		{
			switch( ioreq->io_Command )
			{
				case TD_CHANGESTATE:
				case TD_PROTSTATUS:
				case TD_GETDRIVETYPE:
				case TD_REMCHANGEINT:
				case NSCMD_TD_REMSTATCALLBACK:
				case NSCMD_DEVICEQUERY:
				case CMD_UPDATE:
				case CMD_CLEAR:
				{
					// Handled normally below
					break;
				}

				default:
				{
					USBERROR( "MSD : __myHandle_Begin : Detached, failing cmd $%04lx", (U32) ioreq->io_Command );

					ioreq->io_Error  = TDERR_DiskChanged;
					ioreq->io_Actual = 0;

					MSGPORT_REPLYMSG( ioreq );
					continue;
				}
			}
		}

		switch( ioreq->io_Command )
		{
			case /* 0002 */ CMD_READ:					reply = MSD_Cmd_0002_CMD_Read( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0003 */ CMD_WRITE:					reply = MSD_Cmd_0003_CMD_Write( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0009 */ TD_MOTOR:					reply = MSD_Cmd_0009_TD_Motor( usbbase, msddev, (PTR) ioreq ); break;
			case /* 000E */ TD_CHANGESTATE:				reply = MSD_Cmd_000E_TD_ChangeState( usbbase, msddev, (PTR) ioreq ); break;
			case /* 000F */ TD_PROTSTATUS:				reply = MSD_Cmd_000F_TD_ProtStatus( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0012 */ TD_GETDRIVETYPE:			reply = MSD_Cmd_0012_TD_GetDriveType( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0014 */ TD_ADDCHANGEINT:			reply = MSD_Cmd_0014_TD_AddChangeInt( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0015 */ TD_REMCHANGEINT:			reply = MSD_Cmd_0015_TD_RemChangeInt( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0016 */ TD_GETGEOMETRY:				reply = MSD_Cmd_0016_TD_GetGeometry( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0018 */ TD_READ64:					reply = MSD_Cmd_0018_TD_Read64( usbbase, msddev, (PTR) ioreq ); break;
			case /* 0019 */ TD_WRITE64:					reply = MSD_Cmd_0019_TD_Write64( usbbase, msddev, (PTR) ioreq ); break;
			case /* 001C */ HD_SCSICMD:					reply = MSD_Cmd_001C_HD_SCSICmd( usbbase, msddev, (PTR) ioreq ); break;
			case /* 4000 */ NSCMD_DEVICEQUERY:			reply = MSD_Cmd_4000_NSCmd_DeviceQuery( usbbase, msddev, (PTR) ioreq ); break;
			case /* A004 */ NSCMD_TD_GETGEOMETRY64:		reply = MSD_Cmd_A004_NSCmd_GetGeometry64( usbbase, msddev, (PTR) ioreq ); break;
			case /* A005 */ NSCMD_TD_CHANGEUNIT:		reply = MSD_Cmd_A005_NSCmd_ChangeUnit( usbbase, msddev, (PTR) ioreq ); break;
			case /* A006 */ NSCMD_TD_ADDSTATCALLBACK:	reply = MSD_Cmd_A006_NSCmd_AddStatCallBack( usbbase, msddev, (PTR) ioreq ); break;
			case /* A007 */ NSCMD_TD_REMSTATCALLBACK:	reply = MSD_Cmd_A007_NSCmd_RemStatCallBack( usbbase, msddev, (PTR) ioreq ); break;
			case /* C000 */ NSCMD_TD_READ64:			reply = MSD_Cmd_C000_NSCmd_Read64( usbbase, msddev, (PTR) ioreq ); break;
			case /* C001 */ NSCMD_TD_WRITE64:			reply = MSD_Cmd_0019_TD_Write64( usbbase, msddev, (PTR) ioreq ); break;	// same semantics as TD_WRITE64

			case /* 0004 */ CMD_UPDATE:
			{
				USBERROR( "MSD : _cmd_0004_CMD_UPDATE" );
				reply = TRUE;
				break;
			}

			case /* 0005 */ CMD_CLEAR:
			{
				USBERROR( "MSD : _cmd_0005_CMD_CLEAR" );
				reply = TRUE;
				break;
			}

			default:
			{
				USBERROR( "MSD : __myHandle_Begin : Unknown :  IOReq %p : Command $%04lx", ioreq, ioreq->io_Command );
				ioreq->io_Error = IOERR_NOCMD;
				reply = TRUE;
				break;
			}
		}

		if ( reply )
		{
			MSGPORT_REPLYMSG( ioreq );
		}
	}

	TASK_NAME_LEAVE();
}

#endif
// --
