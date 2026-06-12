
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

static void __myProcess_Free_MsgPorts( struct PrinterUnit *unit )
{
	USBERROR( "Printer : __myProcess_Free_MsgPorts" );

	if ( unit->unit_PRT_Abort_MsgPort )
	{
		FreeSysObject( ASOT_PORT, unit->unit_PRT_Abort_MsgPort );
		unit->unit_PRT_Abort_MsgPortBit = 0;
		unit->unit_PRT_Abort_MsgPort = NULL;
	}

	if ( unit->unit_PRT_Begin_MsgPort )
	{
		FreeSysObject( ASOT_PORT, unit->unit_PRT_Begin_MsgPort );
		unit->unit_PRT_Begin_MsgPortBit = 0;
		unit->unit_PRT_Begin_MsgPort = NULL;
	}
}
