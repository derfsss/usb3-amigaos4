
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

void _usb_Free( struct USB_Struct *us )
{
	MYINFO( "PTP-USB : _usb_Free" );

	if ( us->us_Register )
	{
		USB3_Reg_Unregister( us->us_Register );
		us->us_Register = NULL;
	}
}

// --
