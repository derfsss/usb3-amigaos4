
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

SEC_CODE S32 MSD_Cmd_0012_TD_GetDriveType( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
S32 reply;

	TASK_NAME_ENTER( "MSD : TD_GetDriveType" );
	USBERROR( "MSD : _cmd_0012_TD_GetDriveType" );

	// todo : Convert TD_GETGEOMETRY's dg_DeviceType to DRIVExxx ?

	ioreq->io_Actual = DRIVE3_5;	// hmm proberly not correct
	ioreq->io_Error = 0;
	reply = TRUE;

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
