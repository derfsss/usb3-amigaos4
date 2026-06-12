
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

SEC_CODE void HID_Free( struct USBBase *usbbase, struct intern *in )
{
	USBDEBUG( "HID : HID_Free" );
	TASK_NAME_ENTER( "HID_Free" );

	// --

	if ( in->Register )
	{
		USBDEBUG( "HID_Free              : Unregister" );
		REGISTER_UNREGISTER( in->Register );
		in->Register = NULL;
	}

	// -- Misc Free

	if (( in->Driver_Mode == HID_DMode_Boot ) && ( in->Driver_Type == HID_DType_Keyboard ))
	{
		if ( in->Type.Boot_Keyboard.Timer_Added )
		{
			IO_STOP( & in->Type.Boot_Keyboard.Timer_IOReq );
			in->Type.Boot_Keyboard.Timer_Added = FALSE;
		}

		MSGPORT_FREE( & in->Type.Boot_Keyboard.Timer_MsgPort );
	}

	// --

	MSGPORT_FREE( & in->Input_MsgPort );

	// --

	TASK_NAME_LEAVE();
}

// --
