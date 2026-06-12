
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

SEC_CODE S32 MSD_Cmd_0009_TD_Motor( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
S32 reply;

	TASK_NAME_ENTER( "MSD : TD_Motor" );
	USBERROR( "MSD : _cmd_0009_TD_Motor" );

	// io_Length hold User request to Turn Motor On/off
	// we could store val, and pass Prev state in io_Actual
	// but we are always on, so lets just return 1 in io_Actual

	ioreq->io_Actual = 1;	// Prev Motor State (we are always on)
	ioreq->io_Error = 0;
	reply = TRUE;

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
