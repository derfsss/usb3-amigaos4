
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Misc_RemIntServer( struct USBBase *usbbase, U32 nr, struct Interrupt *i )
{
struct ExecIFace *IExec;

	USBINFO( "Removing Interrupt %ld Server %p", nr, i );

	IExec = usbbase->usb_IExec;

	IExec->RemIntServer( nr, i );
}

// --
