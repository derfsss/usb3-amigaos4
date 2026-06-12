
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

/* Supported commands */

SEC_RODATA static const U16 _DevNSDCmds[] =
{
// Common device commands
/* 0000 */	//	CMD_INVALID,			// Not a real command
/* 0001 */	//	CMD_RESET,
/* 0002 */	CMD_READ,
/* 0003 */	CMD_WRITE,
/* 0004 */	CMD_UPDATE,
/* 0005 */	CMD_CLEAR,
/* 0006 */	//	CMD_STOP,
/* 0007 */	//	CMD_START,
/* 0008 */	//	CMD_FLUSH,
/* 0009	*/	//	CMD_NONSTD,				// Normal first Device Specific Command

// Trackdisk.dev commands
/* 0009 */	TD_MOTOR,					// CMD_NONSTD+0
/* 000A */	//	TD_SEEK,
/* 000B */	//	TD_FORMAT,
/* 000C */	//	TD_REMOVE,
/* 000D */	//	TD_CHANGENUM,
/* 000E */	TD_CHANGESTATE,
/* 000F */	TD_PROTSTATUS,
/* 0010 */	//	TD_RAWREAD,
/* 0011 */	//	TD_RAWWRITE,
/* 0012 */	//	TD_GETDRIVETYPE,
/* 0013 */	//	TD_GETNUMTRACKS,
/* 0014 */	TD_ADDCHANGEINT,
/* 0015 */	TD_REMCHANGEINT,
/* 0016 */	TD_GETGEOMETRY,
/* 0017 */	//	TD_EJECT,
/* 0018 */	//	TD_LASTCOMM,			// not a real command just Last 'Normal' TD entry
/* 0018 */	TD_READ64,					// TD64 Cmd - Unoffical commands
/* 0019 */	TD_WRITE64,					// TD64 Cmd
/* 001A */	//	TD_SEEK64,				// TD64 Cmd
/* 001B */	//	TD_FORMAT64,			// TD64 Cmd
/* 001C */	HD_SCSICMD,					// Direct-SCSI cmd
/* 4000 */	NSCMD_DEVICEQUERY,
/* 8002 */	//	WTD_READ,				// TDF_EXTCOM + CMD_READ
/* 8003 */	//	WTD_WRITE,				// TDF_EXTCOM + CMD_WRITE
/* 8004 */	//	WTD_UPDATE,				// TDF_EXTCOM + CMD_UPDATE,
/* 8005 */	//	WTD_CLEAR,				// TDF_EXTCOM + CMD_CLEAR,
/* 8009 */	//	WTD_MOTOR,				// TDF_EXTCOM + TD_MOTOR,
/* 800A */	//	WTD_SEEK,				// TDF_EXTCOM + TD_SEEK,
/* 800B */	//	WTD_FORMAT,				// TDF_EXTCOM + TD_FORMAT,
/* 8010 */	//	WTD_RAWREAD,			// TDF_EXTCOM + TD_RAWREAD,
/* 8011 */	//	WTD_RAWWRITE,			// TDF_EXTCOM + TD_RAWWRITE,
/* A004 */	NSCMD_TD_GETGEOMETRY64,		// Mounter.lib
/* A005 */	NSCMD_TD_CHANGEUNIT,		// Mounter.lib
/* A006 */	NSCMD_TD_ADDSTATCALLBACK,	// Mounter.lib
/* A007 */	NSCMD_TD_REMSTATCALLBACK,	// Mounter.lib
/* C000 */	NSCMD_TD_READ64,			// NSCMD - New Style Commands
/* C001 */	NSCMD_TD_WRITE64,			// NSCMD
/* C002 */	//	NSCMD_TD_SEEK64,		// NSCMD
/* C003 */	//	NSCMD_TD_FORMAT64,		// NSCMD
/* E000 */	//	NSCMD_ETD_READ64,		// TDF_EXTCOM + NSCMD_TD_READ64
/* E001 */	//	NSCMD_ETD_WRITE64,		// TDF_EXTCOM + NSCMD_TD_WRITE64
/* E002 */	//	NSCMD_ETD_SEEK64,		// TDF_EXTCOM + NSCMD_TD_SEEK64
/* E003 */	//	NSCMD_ETD_FORMAT64,		// TDF_EXTCOM + NSCMD_TD_FORMAT64
	0
};

SEC_CODE S32 MSD_Cmd_4000_NSCmd_DeviceQuery( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct NSDeviceQueryResult *nsd;
S32 reply;

	TASK_NAME_ENTER( "MSD : NSCmd_DeviceQuery" );
	USBERROR( "MSD : _cmd_4000_NSCmd_DeviceQuery" );

	nsd = ioreq->io_Data;

	// The size check is only valid because the current NSDDeviceQueryResult
	// structure is defined as the minimum version of that struct!
	if (( nsd ) && ( ioreq->io_Length >= sizeof( struct NSDeviceQueryResult )))
	{
		nsd->DevQueryFormat		= 0;
		nsd->SizeAvailable		= sizeof( struct NSDeviceQueryResult );
		nsd->DeviceType			= NSDEVTYPE_TRACKDISK;
		nsd->DeviceSubType		= 0;
		nsd->SupportedCommands 	= (PTR) _DevNSDCmds;

		ioreq->io_Actual		= sizeof( struct NSDeviceQueryResult );
		ioreq->io_Error			= 0;
	}
	else
	{
		USBERROR( "MSD : NSCmd_DeviceQuery : Supplied Query buffer is to small" );
		ioreq->io_Error			= IOERR_BADLENGTH;
		ioreq->io_Actual		= 0;
	}

	reply = TRUE;

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
