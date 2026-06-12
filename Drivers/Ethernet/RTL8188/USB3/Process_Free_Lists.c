
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

static void __myProcess_Free_Lists( struct EthernetUnit *unit )
{
	USBERROR( "RTL8188 : __myProcess_Free_Lists" );

	// --

	if ( unit->unit_USB_IOReq_FreeList )
	{
		USBINFO( "RTL8188 : Freeing Tx Free List" );

//		Abort_IOList( unit, unit->unit_QueueList );
//		  Abort_BulkOut( unit, unit->unit_TxCurrentIO );

		FreeSysObject( ASOT_LIST, unit->unit_USB_IOReq_FreeList );
		unit->unit_USB_IOReq_FreeList = NULL;
	}

	// --

	if ( unit->unit_Bulk_Tx_Active )
	{
		USBINFO( "RTL8188 : Freeing Tx Active Array" );

//		Abort_IOList( unit, unit->unit_QueueList );
//		  Abort_BulkOut( unit, unit->unit_TxCurrentIO );

//		FreeSysObject( ASOT_LIST, unit->unit_Bulk_Tx_Active );
//		unit->unit_Bulk_Tx_Active = NULL;
	}

	// --

	if ( unit->unit_PAR_IOReq_WaitingList )
	{
		USBINFO( "RTL8188 : Freeing Tx Waiting List" );

//		Abort_IOList( unit, unit->unit_QueueList );
//		  Abort_BulkOut( unit, unit->unit_TxCurrentIO );

		FreeSysObject( ASOT_LIST, unit->unit_PAR_IOReq_WaitingList );
		unit->unit_PAR_IOReq_WaitingList = NULL;
	}

	// --
}

// --
