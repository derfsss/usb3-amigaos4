
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

SEC_CODE void MSD_Manager_AbortIO( struct DeviceManagerInterface *Self UNUSED, struct IOStdReq *ioreq UNUSED )
{
	TASK_NAME_ENTER( "MSD : MSD_Manager_AbortIO" );
//	USBERROR( "MSD : MSD_Manager_AbortIO" );

	#if 0
	// Fast check, is it pending?
	if ( ioreq->io_Message.mn_Node.ln_Type != NT_MESSAGE )
	{
		goto bailout;
	}

	switch( ioreq->io_Command )
	{

		default:
		case CMD_STOP:
		case CMD_START:
		case NSCMD_DEVICEQUERY:
		{
			SHOWMSG( "Unknown Command (%04lx)", ioreq->io_Command );
			break;
		}
	}

bailout:
	#endif

	TASK_NAME_LEAVE();
	return;
}
