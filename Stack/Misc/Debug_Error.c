
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
#ifdef DO_ERROR

SEC_CODE void VARARGS68K __Debug_Error( struct USBBase *usbbase, STR fmt, ... )
{
struct Task *self;
va_list ap;
PTR base;
STR buf;

	// --

	if ( ! usbbase->_dbg_IExec )
	{
		usbbase->_dbg_IExec = (PTR)(*(struct ExecBase **)4)->MainInterface;
		// Can't fail .. or the system is dead
	}

	// --

	if ( ! usbbase->_dbg_IUility )
	{
		base = usbbase->_dbg_IExec->OpenLibrary( "utility.library", 0 );
		usbbase->_dbg_IUility = (PTR) usbbase->_dbg_IExec->GetInterface( base, "main", 1, NULL );
	}

	// -- 
	// Try and build a buffer

	va_start( ap, fmt );

	if ( usbbase->_dbg_IUility )
	{
		buf = usbbase->_dbg_IUility->VASPrintf( fmt, va_getlinearva( ap, PTR ));
	}
	else
	{
		buf = NULL;
	}

	va_end( ap );

	// --

	usbbase->_dbg_IExec->Disable();

	self = usbbase->_dbg_IExec->FindTask(NULL);

	usbbase->_dbg_IExec->DebugPrintF( "%s%p: %s\n",
		( self != usbbase->_dbg_Task ) ? "--\n" : "", 
		self,
		( buf ) ? buf : (STR) fmt
	);

	usbbase->_dbg_Task = self;

	usbbase->_dbg_IExec->Enable();

	// --

	if ( buf )
	{
		usbbase->_dbg_IExec->FreeVec( buf );
	}

	// --
}

#endif

// --
