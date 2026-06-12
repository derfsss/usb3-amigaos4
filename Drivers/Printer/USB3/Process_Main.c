
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

#include "src/Process_Main_BeginIO.c"

#include "Process_Main_Shutdown.c"
#include "Process_Main_Buffer.c"
#include "Process_Main_Direct.c"

// --

void Process_Main( struct PrinterUnit *unit )
{
U32 wait;
U32 mask;

	USBERROR( "Printer : __myProcess_Main : Enter" );

	wait = SIGBREAKF_CTRL_C;
	wait |= unit->unit_PRT_Begin_MsgPortBit;
	wait |= unit->unit_PRT_Abort_MsgPortBit;
	wait |= unit->unit_Bulk_Tx_Resource->MsgPortBit;

	unit->unit_Running = TRUE;

	while( unit->unit_Running )
	{
		mask = Wait( wait );

		if ( mask & unit->unit_PRT_Abort_MsgPortBit )
		{

		}

		if ( mask & unit->unit_PRT_Begin_MsgPortBit )
		{
			__myProcess_Main_BeginIO( unit );
		}

		if ( mask & unit->unit_Bulk_Tx_Resource->MsgPortBit )
		{
			if ( unit->unit_Buffer_Size )
			{
				__myProcess_Main_Buffer( unit );
			}
			else
			{
				__myProcess_Main_Direct( unit );
			}
		}

		if ( mask & SIGBREAKF_CTRL_C )
		{
			if ( unit->unit_ExitParent )
			{
				__myProcess_Main_Shutdown( unit );
			}
		}
	}

	unit->unit_StartupComplete = FALSE;

	#if 0
	USBERROR( "Lasy byte : $%02lx (%ld)", (S32) unit->unit_LastByteSend, (S32) unit->unit_LastByteSend );
	if (( unit->unit_LastByteSend != '\f' )
	&&	( unit->unit_Detached == 0 ))
	{
		Send_FormFeed( unit );
	}
	#endif

	USBERROR( "Printer : __myProcess_Main : Leave" );
}

// --
