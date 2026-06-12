
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --

SEC_CODE U32 MSD_Manager_Release( struct DeviceManagerInterface *Self )
{
	TASK_NAME_ENTER( "MSD : MSD_Manager_AbortIO" );

	Self->Data.RefCount--;

	TASK_NAME_LEAVE();
	return(	Self->Data.RefCount );
}
