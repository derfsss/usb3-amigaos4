
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Mem_FreeIOBuffer( struct USBBase *usbbase, PTR mem )
{
struct ExecIFace *IExec;

	#ifdef DO_PANIC

	if ( ! usbbase )
	{
		USBPANIC( "Mem_FreeIOBuffer : 1 : NULL Pointer" );
	}

	#endif

	if ( mem )
	{
		IExec = usbbase->usb_IExec;
		IExec->FreeVec( mem );
	}

}

// --
