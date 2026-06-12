
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

static void __myProcess_Free_MsgPorts( struct EthernetUnit *unit )
{
	USBERROR( "RTL8188 : __myProcess_Free_MsgPorts" );

	if ( unit->unit_ETH_Abort_MsgPort )
	{
		FreeSysObject( ASOT_PORT, unit->unit_ETH_Abort_MsgPort );
		unit->unit_ETH_Abort_MsgPortBit = 0;
		unit->unit_ETH_Abort_MsgPort = NULL;
	}

	if ( unit->unit_ETH_Begin_MsgPort )
	{
		FreeSysObject( ASOT_PORT, unit->unit_ETH_Begin_MsgPort );
		unit->unit_ETH_Begin_MsgPortBit = 0;
		unit->unit_ETH_Begin_MsgPort = NULL;
	}
}
