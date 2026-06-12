
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

void _usb_Loop_Stack( struct USB_Struct *us )
{
struct USB3_NotifyMessage *msg;

	while( TRUE )
	{
		msg = (PTR) GetMsg( us->us_Register->Stack_MsgPort );

		if ( ! msg )
		{
			break;
		}

		switch( msg->nm_Type )
		{
			case USBNC_Detached:
			{
				MYINFO( "PTP-USB : Detach msg" );
				us->us_Detached = FALSE;
				break;
			}

			default:
			{
				MYINFO( "PTP-USB : Unknown Stack Message Type (%lu)", msg->nm_Command );
				break;
			}
		}

		ReplyMsg( (PTR) msg );
	}
}

// --
