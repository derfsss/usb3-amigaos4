
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

S32 Unit_Startup( struct EthernetUnit *unit )
{
struct TaskStartMsg msg;
S32 retval;
U32 mask;
S32 sig;

	retval = FALSE;

	if ( ! unit )
	{
		USBERROR( "RTL8188 : Unit_Startup : NULL Pointer" );
		goto bailout;
	}

	USBDEBUG( "RTL8188 : Unit_Startup : UnitNr #%lu", unit->unit_UnitNr );

	// --

	if ( ! Unit_Startup_USB( unit ))
	{
		USBERROR( "RTL8188 : Failed to open usb3.device" );
		goto bailout;
	}

	// --
	// Try and alloc signal
	sig = AllocSignal( -1 );

	msg.Result	= 0;
	msg.Parent	= FindTask( NULL );
	msg.Mask	= ( sig == -1 ) ? ( SIGBREAKF_CTRL_D ) : ( 1UL << sig );
	msg.Unit	= unit;

	// Clear signal
	SetSignal( 0, msg.Mask );

	if ( CreateNewProcTags( 
		NP_Name,		unit->unit_TaskName,
		NP_Child,		FALSE,
		NP_Priority,	5,
		NP_StackSize,	1024 * 32,
		NP_Entry,		Ethernet_Process_Entry,
		NP_UserData,	& msg,
		TAG_END ))
	{
		while( TRUE )
		{
			mask = Wait( msg.Mask );

			if (( mask & msg.Mask ) == msg.Mask )
			{
				break;
			}
		}
	}

	if ( sig != -1 )
	{
		FreeSignal( sig );
	}

	if ( ! msg.Result )
	{
		USBERROR( "RTL8188 : Failed to RTL8188 process" );
		goto bailout;
	}

	// --

	// Startup complete, we can now handle IOReq
	unit->unit_StartupComplete = TRUE;

	// All okay
	retval = TRUE;

bailout:

	USBDEBUG( "RTL8188 : Unit_Startup : Leave : Retval %ld", retval );

	return( retval );
}

// --
