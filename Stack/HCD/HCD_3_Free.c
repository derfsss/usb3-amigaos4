
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "HCD_3_Free_1_Rem_HUB.c"
#include "HCD_3_Free_2_Rem_Int.c"
#include "HCD_3_Free_3_Misc.c"

// --

SEC_CODE static void __myFree( struct USBBase *usbbase, PTR userdata, PTR in_ptr )
{
//struct USB2_DriverNode *dn;
struct USB2_HCDNode *hn;
struct intern *in;

	hn = userdata;
	in = in_ptr;

	USBDEBUG( "__myFree (HCD)           : Enter : HN    %p", hn );
	TASK_NAME_ENTER( "HCD : __myFree" );

	// --

	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBDEBUG( "__myFree (HCD)           : Invalid HCD (%p)", hn );
		goto bailout;
	}

	// --

	__Free_3_Rem_HUB( usbbase, hn, in );

	if ( hn->HCD_Functions.Chip_Stop )
	{
		 hn->HCD_Functions.Chip_Stop( hn );
	}

	__Free_3_Rem_Int( usbbase, hn, in );

	if ( hn->HCD_Functions.Chip_Deinit )
	{
		 hn->HCD_Functions.Chip_Deinit( hn );
	}

	if ( hn->HCD_Functions.Chip_Dealloc )
	{
		 hn->HCD_Functions.Chip_Dealloc( hn );
	}

	__Free_3_Misc( usbbase, hn, in );

	// --

bailout:

	// -- Exit

	USBDEBUG( "__myFree (HCD)           : Leave" );

	TASK_NAME_LEAVE();

	return;
}

// --
