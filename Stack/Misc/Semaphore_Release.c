
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Semaphore_Release( struct USBBase *usbbase UNUSED, struct USB3_Semaphore *us )
{
struct ExecIFace *IExec;

	#ifdef DO_PANIC

	if ( ! us )
	{
		USBPANIC( "Semaphore_Release : 1 : NULL Pointer" );
	}

	if ( us->us_StructID != ID_USB3_SEMA )
	{
		USBPANIC( "Semaphore_Release : 2 : Invalid ID" );
	}

	#endif

	IExec = usbbase->usb_IExec;
	IExec->ReleaseSemaphore( & us->us_Semaphore );
}

// --
