
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

SEC_CODE PTR __Node_Prev( struct USBBase *usbbase UNUSED, PTR cur, STR file UNUSED )
{
PTR prev;

	if ( ! cur )
	{
		USBPANIC( "__Node_Prev : NULL Pointer : Cur %p : (%s)", cur, (file)?file:"<NULL>" );
	}

	if ( cur == (PTR) 0x55555555 )
	{
		USBPANIC( "__Node_Prev : Free'd Pointer : Cur %p : (%s)", cur, (file)?file:"<NULL>" );
	}

	prev = ((struct USB3_Node *)cur)->un_Prev;

	if ( prev == (PTR) 0x55555555 )
	{
		USBPANIC( "__Node_Prev : Free'd Pointer : Cur %p : Prev %p : (%s)", cur, prev, (file)?file:"<NULL>" );
	}

	return( prev );
}

#endif
// --
