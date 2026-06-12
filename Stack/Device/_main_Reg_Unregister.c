
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
/* -- Main Interface -- */

static void _main_Reg_Unregister( struct USB2IFace *Self, struct RealRegister *reg )
{
struct USBBase *usbbase;

	usbbase = (PTR) Self->Data.LibBase;
	USBERROR( "USB2 Stack : _main_Reg_Unregister" );

	if ( REGISTER_VALID( reg ) != VSTAT_Okay )
	{
		USBERROR( "Error invalid Register Pointer" );
		goto bailout;
	}

	REGISTER_UNREGISTER( reg );

bailout:

	return;
}

// --

