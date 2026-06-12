
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE U32 __Task_Wait( struct USBBase *usbbase, U32 waitmask )
{
struct ExecIFace *IExec;
U32 mask;

	IExec = usbbase->usb_IExec;
	mask = IExec->Wait( waitmask );

	return( mask );
}

// --
