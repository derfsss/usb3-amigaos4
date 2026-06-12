
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
// Safe String Length, can handle NULL Pointer

SEC_CODE U32 __Misc_StrLength( struct USBBase *usbbase UNUSED, STR txt )
{
STR str;
U32 len;

	TASK_NAME_ENTER( "__Misc_StrLength" );

	len = 0;

	if ( ! txt )
	{
		goto bailout;
	}

	str = txt;

	while( *str )
	{
		str++;
	}

	len = str - txt;

bailout:

	TASK_NAME_LEAVE();

	return( len );
}

// --
