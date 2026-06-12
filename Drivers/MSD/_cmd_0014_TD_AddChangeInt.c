
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

SEC_CODE S32 MSD_Cmd_0014_TD_AddChangeInt( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct MSDDisk *msddisk;
S32 reply;

	TASK_NAME_ENTER( "MSD : _cmd_0014_TD_AddChangeInt" );
	USBERROR( "MSD : _cmd_0014_TD_AddChangeInt" );

	// We return the IOReq, on Stat change or RemChangeInt
	reply = FALSE;

	msddisk = (PTR) ioreq->io_Unit;

	SEMAPHORE_OBTAIN( & msddisk->msddisk_MSDUnit_Semaphore );

	NODE_ADDTAIL( & msddisk->msddisk_MSDUnit_ChangeList, ioreq );

	SEMAPHORE_RELEASE( & msddisk->msddisk_MSDUnit_Semaphore );

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
