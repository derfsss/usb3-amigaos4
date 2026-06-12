
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
#ifdef DO_PANIC

SEC_CODE void VARARGS68K __Debug_Panic( struct USBBase *usbbase UNUSED, STR filename, U32 linenr, STR fmt, ... )
{
struct UtilityIFace *_IU;
struct ExecIFace *_IE;
STR printbuf;
PTR base;
STR buf;
va_list ap;

	_IE	 = (PTR)(*(struct ExecBase **)4)->MainInterface;
	_IE->DebugPrintF( "USBPANIC!!\n" );
	base = (PTR) _IE->OpenLibrary( "utility.library", 0 );
	_IU  = (PTR) _IE->GetInterface( base, "main", 1, NULL );

	// ---

	va_start( ap, fmt );

	buf = _IU->VASPrintf( fmt, va_getlinearva( ap, PTR ));

	va_end( ap );

	// ---

	printbuf = ( buf ) ? buf : (STR) fmt;

	_IE->Disable();
	_IE->DebugPrintF( "\n\n-- USBPANIC --\n" );
	_IE->DebugPrintF( "%s:%04ld:%s\n", filename, linenr, printbuf );
	_IE->DebugPrintF( "-- USBPANIC --\n\n\n" );
	_IE->Wait( 0 );

	// ---
}

#endif
// --
