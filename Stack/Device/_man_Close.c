
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

static PTR _manager_Close(
	struct DeviceManagerInterface *Self,
	struct IORequest *ioreq )
{
struct USB3_DeviceUnit *unit;
struct USBBase *usbbase;
PTR seglist;

	TASK_NAME_ENTER( "USB3: _manager_Close" );

	usbbase = (PTR) Self->Data.LibBase;
//	usbbase->usb_IExec->DebugPrintF( "manager_close\n" );

	SEMAPHORE_OBTAIN( & usbbase->usb_OpenCloseSemaphore );

	unit = (PTR) ioreq->io_Unit;

	if (( unit == NULL ) 
	||	( unit == (PTR) -1 ))
	{
		// Error

		seglist = NULL;
	}
	else
	{
		// Okay

		if ( unit->unit_Unit.unit_OpenCnt > 0 )
		{
			unit->unit_Unit.unit_OpenCnt--;
		}

		if ( usbbase->usb_Library.lib_OpenCnt > 0 )
		{
			usbbase->usb_Library.lib_OpenCnt--;
		}

		if (( unit->unit_Unit.unit_OpenCnt == 0 )
		&& 	( usbbase->usb_Library.lib_OpenCnt == 0 )
		&&	( usbbase->usb_Library.lib_Flags & LIBF_DELEXP ))
		{
			seglist = _manager_Expunge( Self );
		}
		else
		{
			seglist = NULL;
		}

		// Make sure, the ioreq is not used again
		ioreq->io_Device = (PTR) -1;
		ioreq->io_Unit	 = (PTR) -1;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_OpenCloseSemaphore );

	/* Must return NULL or the SegList */

	TASK_NAME_LEAVE();
	return( seglist );
}

// --
