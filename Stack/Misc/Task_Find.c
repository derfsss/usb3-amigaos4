
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Task_Find( struct USBBase *usbbase )
{
struct ExecIFace *IExec;
PTR retval;

	IExec = usbbase->usb_IExec;
	retval = (PTR) IExec->FindTask( NULL );

	return( retval );
}

// --
