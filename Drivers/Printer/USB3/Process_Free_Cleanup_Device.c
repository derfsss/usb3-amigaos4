
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

static void __myProcess_CleanupDevice( struct PrinterUnit *unit )
{
	USBERROR( "Printer : __myProcess_CleanupDevice" );

	#if 1
	// Unregister will free EPRes, so this is just for testing
	if ( unit->unit_Bulk_Tx_Resource )
	{
		USB3_EPRes_Release( unit->unit_Bulk_Tx_Resource );
		unit->unit_Bulk_Tx_Resource = NULL;
	}
	#endif

	if ( unit->unit_USB_Register )
	{
		USB3_Reg_Unregister( unit->unit_USB_Register );
		unit->unit_USB_Register = NULL;
	}
}

// --
