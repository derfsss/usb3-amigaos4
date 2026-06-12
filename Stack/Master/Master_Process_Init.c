
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

SEC_CODE S32 __myProcess_Init( struct USBBase *usbbase, struct intern *in, S32 Promote )
{
struct DOSIFace *IDOS;
S32 retval;
S32 proc;

	retval = FALSE;

	// --

	if ( ! __myTask_Def_Init( usbbase, in, Promote ))
	{
		USBERROR( "__myProcess_Init (Master) : 1 : Error" );
		goto bailout;
	}

	// --
	// Prevent DOS opening any windows

	proc = MISC_SETPROCWINDOW( -1 );

	IDOS = usbbase->usb_IDOS;
	usbbase->usb_DriverDirLock = IDOS->Lock( DRIVERDIR, ACCESS_READ );

	MISC_SETPROCWINDOW( proc );

	// --

	if ( usbbase->usb_DriverDirLock )
	{
		__myProcess_ScanDriverDir( usbbase, in );
	}
	else
	{
		usbbase->usb_IExec->DebugPrintF( "Failed to get dir lock on : %s\n", DRIVERDIR );
	}

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --
