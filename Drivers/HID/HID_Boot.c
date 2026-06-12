
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

SEC_CODE void HID_Boot( struct USBBase *usbbase, struct intern *in )
{
U32 mask;
U32 wait;

	USBDEBUG( "HID_Boot : Entry" );
	TASK_NAME_ENTER( "HID_Boot" );

	// --

	wait = SIGBREAKF_CTRL_C;
	wait |=	in->Res_Interrupt->MsgPortBit;
//	wait |=	in->Register->reg_Public.Stack_MsgPortBit;

	if ( in->Driver_Type == HID_DType_Keyboard )
	{
		USBDEBUG( "HID_Boot : HID_Boot_DoKeyboardTimer : Mask $%08lx", in->Type.Boot_Keyboard.Timer_MsgPort.ump_Signal.sig_Signal_Mask );
		wait |= in->Type.Boot_Keyboard.Timer_MsgPort.ump_Signal.sig_Signal_Mask;
	}

	in->Running = TRUE;

	while( in->Running )
	{
		//		USBDEBUG( "HID_Boot : Wait $%08lx", wait );
		mask = TASK_WAIT( wait );
//		USBDEBUG( "HID_Boot : Mask $%08lx", mask );

		if (( in->Driver_Type == HID_DType_Keyboard )
		&&	( mask & in->Type.Boot_Keyboard.Timer_MsgPort.ump_Signal.sig_Signal_Mask ))
		{
			TASK_NAME_ENTER( "HID_Boot : Handle HID_Boot_DoKeyboardTimer" );
			HID_Boot_DoKeyboardTimer( usbbase, in );
			TASK_NAME_LEAVE();
		}

		if ( mask &	in->Res_Interrupt->MsgPortBit )
		{
//			USBDEBUG( "HID_Boot : Got Int" );
			/**/ if ( in->Driver_Type == HID_DType_Mouse )
			{
				TASK_NAME_ENTER( "HID_Boot : Handle HID_Boot_Mouse" );
				HID_Boot_Mouse( usbbase, in );
				TASK_NAME_LEAVE();
			}
			else if ( in->Driver_Type == HID_DType_Keyboard )
			{
				TASK_NAME_ENTER( "HID_Boot : Handle HID_Boot_Keyboard" );
				HID_Boot_Keyboard( usbbase, in );
				TASK_NAME_LEAVE();
			}
			else
			{
				USBDEBUG( "HID_Boot : Invalid Driver Type (%ld)", in->Driver_Type );
				in->Running = FALSE;
			}
		}

		if ( mask &	in->Register->reg_Public.Stack_MsgPortBit )
		{
			USBDEBUG( "HID : Got Stack Bit" );

			HID_Boot_Handle_Stack( usbbase, in );
		}

		if ( mask & SIGBREAKF_CTRL_C )
		{
			USBDEBUG( "HID_Boot : Got CTRL+C : Exit Signal" );

			in->Running = FALSE;
		}
	}


	// --

	USBDEBUG( "HID_Boot : Exit" );
	TASK_NAME_LEAVE();
}

// --
