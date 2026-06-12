
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __Signal( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct intern *in UNUSED )
{
struct RealFunctionNode *fn;

	// Send Shutdown signal too RootHub
	// its the only Driver around we control

	USBINFO( "__myMain (HCD)           : __Signal (2) ::" );
	TASK_NAME_ENTER( "HCD Main : __Signal" );

	fn = hn->hn_Function_Array[0];

	TASK_STOP_FN( fn );

	TASK_NAME_LEAVE();
}

// --
