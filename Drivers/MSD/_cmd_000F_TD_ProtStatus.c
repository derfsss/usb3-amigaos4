
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

SEC_CODE S32 MSD_Cmd_000F_TD_ProtStatus( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
//struct MSDDisk *msddisk;
S32 reply;

	TASK_NAME_ENTER( "MSD : _cmd_000F_TD_ProtStatus" );
	USBERROR( "MSD : _cmd_000F_TD_ProtStatus" );

	#if 0
	io_Actual - if io_Error is 0, this tells you whether the disk in the
				drive is write-protected. 0 means the disk is NOT write-
				protected, while any other value indicates it is.
	#endif

//	msddisk = (PTR) ioreq->io_Unit;

	#warning fix me
//	ioreq->io_Actual = ( msddisk->msdunit_WriteProtected ) ? TRUE : FALSE ;
	ioreq->io_Actual = FALSE;
	ioreq->io_Error = 0;
	reply = TRUE;

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
