
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

SEC_CODE S32 MSD_Cmd_000E_TD_ChangeState( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
S32 reply;

	TASK_NAME_ENTER( "MSD : _cmd_000E_TD_ChangeState" );
	USBERROR( "MSD : _cmd_000E_TD_ChangeState" );

	#if 0
	io_Actual - if io_Error is 0, this tells you whether a disk is in
				the drive. 0 means there is a disk, while anything else
				indicates there is no disk
	#endif

	// 0 = Attached, 1 = Removed
	ioreq->io_Actual = ( msddev->msddev_Detached ) ? TRUE : FALSE ;
	ioreq->io_Error = 0;
	reply = TRUE;

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
