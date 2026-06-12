
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Semaphore_Init( struct USBBase *usbbase, struct USB2_Semaphore *us )
{
struct ExecIFace *IExec;

	#ifdef DO_PANIC

	if ( ! us )
	{
		USBPANIC( "Semaphore_Init : 1 : NULL Pointer" );
	}

	if (( us->us_StructID != 0 ) && ( us->us_StructID != ID_USB2_FREED ))
	{
		USBPANIC( "Semaphore_Init : 2 : IN    use" );
	}

	#endif

	IExec = usbbase->usb_IExec;
	IExec->InitSemaphore( & us->us_Semaphore );
	us->us_StructID = ID_USB2_SEMA;
}

// --
