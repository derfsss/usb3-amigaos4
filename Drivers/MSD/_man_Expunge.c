
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

SEC_CODE PTR MSD_Manager_Expunge( struct DeviceManagerInterface *Self UNUSED )
{
	TASK_NAME_ENTER( "MSD : MSD_Manager_Expunge" );
	TASK_NAME_LEAVE();
	return(	NULL );
}
