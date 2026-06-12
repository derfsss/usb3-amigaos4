
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Misc_RemResetCallback( struct USBBase *usbbase, struct Interrupt *i )
{
struct ExecIFace *IExec;

	USBINFO( "Removing Reset Callback %p", i );

	IExec = usbbase->usb_IExec;

	IExec->RemResetCallback( i );
}

// --
