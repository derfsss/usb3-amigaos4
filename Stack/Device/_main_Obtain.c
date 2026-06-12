
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static U32 _main_Obtain( struct Interface *Self )
{
	Self->Data.RefCount--;

	return( Self->Data.RefCount );
}

// --
