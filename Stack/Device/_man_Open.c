
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 _manager_Open(
	struct DeviceManagerInterface *Self,
	struct IORequest *ioreq,
	U32 unit_number,
	U32 flags UNUSED )
{
struct USB3_DeviceUnit *unit;
struct USBBase *usbbase;

	TASK_NAME_ENTER( "USB3: _manager_Open" );

	usbbase = (PTR) Self->Data.LibBase;
//	usbbase->usb_IExec->DebugPrintF( "manager_open\n" );

	SEMAPHORE_OBTAIN( & usbbase->usb_OpenCloseSemaphore );

	if (( unit_number > 0 )
	||	( ioreq->io_Message.mn_Length < sizeof( struct IORequest ) ))
	{
		// Error

		ioreq->io_Device = NULL;
		ioreq->io_Error = IOERR_OPENFAIL;
	}
	else
	{
		// Okay

		unit = & usbbase->usb_OpenUnit;
		unit->unit_Unit.unit_OpenCnt++;

		ioreq->io_Unit = & unit->unit_Unit;
		ioreq->io_Message.mn_Node.ln_Type = NT_REPLYMSG;

		usbbase->usb_Library.lib_Flags &= ~LIBF_DELEXP;
		usbbase->usb_Library.lib_OpenCnt++;

		ioreq->io_Error = 0;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_OpenCloseSemaphore );

	TASK_NAME_LEAVE();
	return( ioreq->io_Error );
}

// --
