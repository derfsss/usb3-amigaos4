
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"

// --

static PTR _manager_Expunge( struct DeviceManagerInterface *Self )
{
struct ExecIFace *IExec;
struct USBBase *usbbase;
PTR seglist;

	TASK_NAME_ENTER( "USB3: _manager_Expunge" );

	usbbase = (PTR) Self->Data.LibBase;

	IExec = usbbase->usb_IExec;
	IExec->DebugPrintF( "manager_expunge\n" );

	if ( ! usbbase->usb_Library.lib_OpenCnt )
	{
		seglist = usbbase->usb_SegList;

		ROMExit( usbbase, __FILE__ );

		IExec->Remove( (PTR) usbbase );
		IExec->DeleteLibrary( (PTR) usbbase );
	}
	else
	{
		usbbase->usb_Library.lib_Flags |= LIBF_DELEXP;
		seglist = NULL;
	}

	/* Must return NULL or the SegList */

	TASK_NAME_LEAVE();
	return( seglist );
}

// --
