
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Init_1_Add_HUB( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct intern *in UNUSED )
{
struct RealFunctionNode *fn;
S32 retval;

	retval = FALSE;

	// -- Add Root HUB Entry

	fn = FUNCTION_ALLOC( hn, NULL, hn->hn_MaxSpeed, 0 );

	if ( ! fn )
	{
		USBERROR( "__myInit (HCD)           : Error: Allocating RootHub Function" );
		goto bailout;
	}

	hn->hn_Function_Array[0] = fn;
	hn->hn_EndPoint_Array[0] = fn->fkt_ControlEndPoint;
	hn->hn_EndPoint_Array[16] = fn->fkt_ControlEndPoint;

	retval = TRUE;

bailout:

	return( retval );
}

// --
