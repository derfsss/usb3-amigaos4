
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

SEC_CODE S32 MSD_Cmd_A006_NSCmd_AddStatCallBack( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct MSDDisk *msddisk;
S32 reply;

	TASK_NAME_ENTER( "MSD : NSCmd_AddStatCallBack" );
	USBERROR( "MSD : _cmd_A006_NSCmd_AddStatCallBack" );

	reply = FALSE;

	msddisk = (PTR) ioreq->io_Unit;

	SEMAPHORE_OBTAIN( & msddisk->msddisk_MSDUnit_Semaphore );

	NODE_ADDTAIL( & msddisk->msddisk_MSDUnit_StatList, ioreq );

	SEMAPHORE_RELEASE( & msddisk->msddisk_MSDUnit_Semaphore );

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
