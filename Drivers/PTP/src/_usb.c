
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

U32 _usb_Entry( struct USB3DriverIFace *Self, struct USB3_DriverMessage *msg )
{
struct USB_Struct us;
S32 stat;

	MYINFO( "PTP-USB : _manager_Init" );

	memset( & us, 0, sizeof( struct USB_Struct ));

	us.us_StartMsg = msg;

	IUSB3 = msg->IUSB3;

	stat = _usb_Init( & us );

	if ( stat )
	{
		_usb_Loop( & us );
	}

	_usb_Free( & us );

	return( stat );
}

// --
