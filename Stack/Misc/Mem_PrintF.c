
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE STR VARARGS68K __Mem_PrintF( struct USBBase *usbbase, STR fmt, ... )
{
struct UtilityIFace *IUtility;
va_list ap;
STR str;

	#ifdef DO_DEBUG

	if (( ! usbbase ) || ( ! fmt ))
	{
		USBPANIC( "Error: Mem_PrintF( %p, %p )", usbbase, fmt );
	}

	#endif

	if ( fmt )
	{
		va_start( ap, fmt );

		IUtility = usbbase->usb_IUtility;
		str = IUtility->VASPrintf( fmt, va_getlinearva( ap, STR ));
//		str = NULL;

		va_end( ap );
	}
	else
	{
		str = NULL;
	}

	#ifdef DO_DEBUG
	if ( str )
	{
		USBDEBUG( "--> %s", str );
	}
	#endif

	return( str );
}

// --
