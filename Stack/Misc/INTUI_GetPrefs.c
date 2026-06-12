
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Misc_GetIntuiPrefs( struct USBBase *usbbase, PTR buf, U32 size )
{
struct IntuitionIFace *IIntuition;
PTR retval;

	USBDEBUG( "Get Intui Prefs( buf %p, size %ld )", buf, size );

	IIntuition = usbbase->usb_IIntuition;

	if ( ! IIntuition )
	{

	}

	if ( IIntuition )
	{
		retval = IIntuition->GetPrefs( buf, size );
	}
	else
	{
		USBDEBUG( "__Misc_GetIntuiPrefs : IIntuition : NULL Pointer" );
		retval = NULL;
	}

	return( retval );
}
