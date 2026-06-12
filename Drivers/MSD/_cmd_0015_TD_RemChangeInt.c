
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

SEC_CODE S32 MSD_Cmd_0015_TD_RemChangeInt( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct MSDDisk *msddisk;
S32 reply;
PTR node;

	TASK_NAME_ENTER( "MSD : _cmd_0015_TD_RemChangeInt" );
	USBERROR( "MSD : _cmd_0015_TD_RemChangeInt" );

	reply = TRUE;

	msddisk = (PTR) ioreq->io_Unit;

	SEMAPHORE_OBTAIN( & msddisk->msddisk_MSDUnit_Semaphore );

	node = msddisk->msddisk_MSDUnit_ChangeList.uh_Head;

	while( node )
	{
		if ( node == ioreq )
		{
			break;
		}
		else
		{
			node = Node_Next( node );
		}
	}

	if ( ! node )
	{
		USBERROR( "MSD : Unknwon IOReq %p", ioreq );
		ioreq->io_Error = TDERR_NotSpecified;
//		reply = FALSE;
	}
	else
	{
		NODE_REMNODE( & msddisk->msddisk_MSDUnit_ChangeList, node );
		ioreq->io_Error = 0;
//		reply = TRUE;
	}

	SEMAPHORE_RELEASE( & msddisk->msddisk_MSDUnit_Semaphore );

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
