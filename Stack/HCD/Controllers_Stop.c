
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __HCD_Controllers_Stop( struct USBBase *usbbase )
{
struct USB3_HCDNode *next;
struct USB3_HCDNode *hn;

	USBINFO( "__HCD_Controllers_Stop   :  1 : Send CTRL to HCDs" );
	TASK_NAME_ENTER( "HCD : __HCD_Controllers_Stop" );

	hn = Header_Head( & usbbase->usb_HCDHeader );

	while( hn )
	{
		next = Node_Next( hn );
		// --
		TASK_STOP_TN( hn->hn_Task );
		// --
		hn = next;
	}

	USBINFO( "__HCD_Controllers_Stop   :  2 : Wait for HCDs to stop" );

	ASYNC_WAIT( & usbbase->usb_HCDASync );

	USBINFO( "__HCD_Controllers_Stop   :  3 : HCDs have stopped, now free HN structs" );

	while( TRUE )
	{
		hn = NODE_REMHEAD( & usbbase->usb_HCDHeader );

		if ( ! hn )
		{
			break;
		}

		USBINFO( "__HCD_Controllers_Stop   :  4 : Freeing hn %p", hn );

		HCD_FREE( hn );
	}

	USBINFO( "__HCD_Controllers_Stop   :  5 : Exit" );

	TASK_NAME_LEAVE();
}

// --
