
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

static void __myProcess_Main_Shutdown( struct PrinterUnit *unit )
{
U8 data[1];

	USBERROR( "Printer : Got shutdown signal" );

	if ( unit->unit_Shutdown )
	{
		USBERROR( "Printer : Shutdown allready inprocess" );
		goto bailout;
	}

	// Mark that we have started the shutdown process
	unit->unit_Shutdown = TRUE;

	// Flush Buffer, if in buffer mode
	if ( unit->unit_Buffer_Size )
	{
		USBERROR( "Last byte : $%02lx (%lu)", (U32) unit->unit_LastByteSend, (U32) unit->unit_LastByteSend );
		if (( unit->unit_LastByteSend != '\f' ) && ( ! unit->unit_Detached ))
		{
			data[0] = '\f';
			Buffer_Write( unit, data, 1 );
		}

		Buffer_Flush( unit );
		Buffer_Schedule( unit );
	}
	else
	{
		Direct_Schedule( unit );
	}

bailout:

	return;
}
