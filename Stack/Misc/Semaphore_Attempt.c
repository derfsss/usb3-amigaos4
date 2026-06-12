
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Semaphore_Attempt( struct USBBase *usbbase UNUSED, struct USB3_Semaphore *us )
{
struct ExecIFace *IExec;
S32 got_lock;

	got_lock = FALSE;

	#ifdef DO_PANIC

	if ( ! us )
	{
		USBPANIC( "__Semaphore_Attempt : 1 : NULL Pointer" );
	}

	#endif

	if (( us->us_StructID == 0 ) 
	||	( us->us_StructID == ID_USB3_FREED ))
	{
		SEMAPHORE_INIT( us );
	}

	#ifdef DO_PANIC

	if ( us->us_StructID != ID_USB3_SEMA )
	{
		USBPANIC( "__Semaphore_Attempt : 2 : Invalid ID" );
	}

	#endif

	IExec = usbbase->usb_IExec;
	got_lock = IExec->AttemptSemaphore( & us->us_Semaphore );

	return( got_lock );
}

// --
