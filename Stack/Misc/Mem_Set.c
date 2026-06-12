
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Mem_Set( struct USBBase *usbbase, PTR mem, U32 value, U32 size )
{
struct UtilityIFace *IUtility;

	#ifdef DO_PANIC

	if (( ! usbbase ) || ( ! mem ))
	{
		USBPANIC( "Mem_Set : 1 : NULL Pointer" );
	}

	#endif

	if ( size > 0 )
	{
		IUtility = usbbase->usb_IUtility;
		IUtility->SetMem( mem, value, size );
	}
}

// --
