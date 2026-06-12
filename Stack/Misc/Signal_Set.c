
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE U32 __Task_SetSignal( struct USBBase *usbbase, U32 a, U32 b )
{
struct ExecIFace *IExec;
U32 mask;

	IExec = usbbase->usb_IExec;
	mask = IExec->SetSignal( a, b );

	return( mask );
}

// --
