
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

#include "Process_Free_MsgPorts.c"
#include "Process_Free_Cleanup_Device.c"
#include "Process_Free_Lists.c"

static void __myProcess_Free( struct EthernetUnit *unit )
{
	USBERROR( "RTL8188 : __myProcess_Free" );

	__myProcess_Free_MsgPorts( unit );

	__myProcess_CleanupDevice( unit );

	__myProcess_Free_Lists( unit );
}

// --
