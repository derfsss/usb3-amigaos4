
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "HCD__myHandle_Abort.c"
#include "HCD__myHandle_Begin.c"
#include "HCD__myHandle_HCD.c"
#include "HCD__myHandle_WatchDog.c"
#include "HCD__myHandle_Reset.c"

// --

#include "HCD_2_Main_1_Normal.c"
#include "HCD_2_Main_2_Signal.c"
#include "HCD_2_Main_3_Stopping.c"
#include "HCD_2_Main_4_Shutdown.c"

// --

SEC_CODE static void __myMain( struct USBBase *usbbase, PTR userdata, PTR in_ptr )
{
struct USB3_HCDNode *hn;
struct intern *in;

	hn = userdata;
	in = in_ptr;

	TASK_NAME_ENTER( "HCD Main : __myMain" );
	USBDEBUG( "__myMain (HCD)           : Enter (0) : HN %p", hn );

	// --

	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBDEBUG( "__myMain (HCD)           : Invalid HCD (%p)", hn );
		goto bailout;
	}

	// --
	// Normal running loop
	__Normal(	usbbase, hn, in );

	// --
	// Signal subtasks to shutdown
	__Signal( 	usbbase, hn, in );

	// --
	// Stopping loop, wait for Resources to be released
	__Stopping( usbbase, hn, in );

	// --
	// Start shutdown
	__Shutdown( usbbase, hn, in );

	// --

bailout:

	// -- Exit

	USBDEBUG( "__myMain (HCD)           : Leave (9) : HN    %p", hn );

	TASK_NAME_LEAVE();

	return;
}

// --
