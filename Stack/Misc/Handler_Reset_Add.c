
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Misc_AddResetCallback( struct USBBase *usbbase, struct Interrupt *i )
{
struct ExecIFace *IExec;
S32 retval;

	USBINFO( "Adding Reset Callback %p", i );

	IExec = usbbase->usb_IExec;

	retval = IExec->AddResetCallback( i );

	return( retval );
}

// --
