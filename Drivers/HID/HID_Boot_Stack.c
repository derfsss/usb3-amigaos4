
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "HID.h"

// --

SEC_CODE void HID_Boot_Handle_Stack( struct USBBase *usbbase, struct intern *in )
{
struct USB2_NotifyMessage *msg;
S32 stop;

	TASK_NAME_ENTER( "HID : HID_Boot_Handle_Stack" );
	USBERROR( "HID : HID_Boot_Handle_Stack" );

	stop = FALSE;

	while(( msg = MSGPORT_GETMSG( & in->Register->reg_MsgPort )))
	{
		switch( msg->nm_Type )
		{

			default:
			{
				USBDEBUG( "HID : Unknown Message Type (%lu)", msg->nm_Command );
				break;
			}
		}

		MSGPORT_REPLYMSG( msg );
	}

	if ( stop )
	{

	}

	TASK_NAME_LEAVE();
}

// --
