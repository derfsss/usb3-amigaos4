
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
#if 0
//#ifdef DO_PANIC

SEC_CODE PTR __Node_Next( struct USBBase *usbbase UNUSED, PTR cur, STR file UNUSED )
{
PTR next;

	if ( ! cur )
	{
		USBPANIC( "__Node_Next : NULL Pointer : Cur %p : (%s)", cur, (file)?file:"<NULL>" );
	}

	if ( cur == (PTR) 0x55555555 )
	{
		USBPANIC( "__Node_Next : Free'd Pointer : Cur %p : (%s)", cur, (file)?file:"<NULL>" );
	}

	next = ((struct USB2_Node *)cur)->un_Next;

	if ( next == (PTR) 0x55555555 )
	{
		USBPANIC( "__Node_Next : Free'd Pointer : Cur %p : Next %p : (%s)", cur, next, (file)?file:"<NULL>" );
	}

	return( next );
}

#endif
// --
