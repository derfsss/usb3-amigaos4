
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static U32 _manager_Obtain( struct Interface *Self )
{
	TASK_NAME_ENTER( "USB3: _manager_Obtain" );

	Self->Data.RefCount++;

	TASK_NAME_LEAVE();
	return( Self->Data.RefCount );
}

// --
