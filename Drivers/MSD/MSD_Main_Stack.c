
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --

SEC_CODE void MSD_Main__myHandle_Stack( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED )
{
struct USB3_NotifyMessage *msg;
S32 stop;

	TASK_NAME_ENTER( "MSD : __myHandle_Stack" );
	USBERROR( "MSD : __myHandle_Stack" );

	stop = FALSE;

	while(( msg = MSGPORT_GETMSG( & msddev->msddev_Register->reg_MsgPort )))
	{
		switch( msg->nm_Type )
		{

			default:
			{
				USBDEBUG( "MSD : Unknown Message Type (%lu)", msg->nm_Command );
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
