
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __Shutdown( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct intern *in UNUSED )
{
struct RealFunctionNode *fn;

	// Yes all should have been shutdown now
	// lets free RootHub Function (not the driver)

	USBINFO( "__myMain (HCD)           : __Shutdown (4) ::" );
	TASK_NAME_ENTER( "HCD Main : __Shutdown" );

	fn = hn->hn_Function_Array[0];

	if ( FUNCTION_FREE(fn) == FSTAT_Okay )
	{
		hn->hn_Function_Array[0] = NULL;
	}
	else
	{
		USBERROR( "__myMain (HCD)           : Index #%ld :: --> Unable to free RootHub Function", hn->hn_HCDIndex );
	}

	TASK_NAME_LEAVE();
}

// --
