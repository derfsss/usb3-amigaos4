
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

struct hhh
{
	U16		USBButton;
	U16		IEQual;
	U16		IECode;
};

SEC_RWDATA struct hhh ooo[] =
{
{ BOOTMOUSE_BUTTONS_LEFT,		IEQUALIFIER_LEFTBUTTON,		IECODE_LBUTTON },
{ BOOTMOUSE_BUTTONS_RIGHT,		IEQUALIFIER_RBUTTON,		IECODE_RBUTTON },
{ BOOTMOUSE_BUTTONS_MIDDLE,		IEQUALIFIER_MIDBUTTON,		IECODE_MBUTTON },
{ BOOTMOUSE_BUTTONS_4TH,		0,							IECODE_4TH_BUTTON },
{ BOOTMOUSE_BUTTONS_5TH,		0,							IECODE_5TH_BUTTON },
{ 0, 0, 0 }
};

// hmmm :: can I send  " 0 | IECODE_UP_PREFIX "  for 4 & 5th button

SEC_CODE void HID_Boot_Mouse_Buttons( struct USBBase *usbbase, struct intern *in, struct USB3_BootMouse *report )
{
struct InputEvent *Event;
struct IOStdReq *ioreq;
U16 code;
U16 new;	// new Button mask
U16 old;	// old Button mask
U16 chg;	// Change Mask
U32 pos;

	TASK_NAME_ENTER( "HID_Boot_Mouse : HID_Boot_Mouse_Buttons" );

	old = in->Type.Boot_Mouse.Buttons;
	new = report->Buttons;
	chg = ( new ^ old ) & BOOTMOUSE_BUTTONS_MASK;

	if ( chg )
	{
		pos = 0;

		while( TRUE )
		{
			// End of Struct ?
			if ( ! ooo[pos].USBButton )
			{
				break;
			}

			// Button Change ?
			if ( ooo[pos].USBButton & chg )
			{
				if ( ooo[pos].USBButton & new )
				{
					in->Qualifiers |= ooo[pos].IEQual;
					code = ooo[pos].IECode;
				}
				else
				{
					in->Qualifiers &= ~ooo[pos].IEQual;
					code = ooo[pos].IECode | IECODE_UP_PREFIX;
				}

				Event = & in->Input_Event;
				Event->ie_NextEvent	= NULL;
				Event->ie_Class		= IECLASS_RAWMOUSE;
				Event->ie_SubClass	= 0;
				Event->ie_Code		= code;
				Event->ie_Qualifier	= in->Qualifiers;
				Event->ie_X			= 0;
				Event->ie_Y			= 0;

				ioreq = & in->Input_IOReq;
				ioreq->io_Command	= IND_ADDEVENT;
				ioreq->io_Data		= Event;
				ioreq->io_Length	= sizeof( struct InputEvent );

				IO_DO( ioreq );
			}

			// Next struct Entry
			pos++;
		}

		// --
		// Store Current Mask
		in->Type.Boot_Mouse.Buttons = new;
	}

	TASK_NAME_LEAVE();
}

// --

SEC_CODE static void myHID_Boot_Mouse_Movement( struct USBBase *usbbase, struct intern *in, struct USB3_BootMouse *report )
{
struct InputEvent *Event;
struct IOStdReq *ioreq;

	TASK_NAME_ENTER( "HID_Boot_Mouse : myHID_Boot_Mouse_Movement" );

	if (( report->X ) || ( report->Y ))
	{
		Event = & in->Input_Event;
		Event->ie_NextEvent	= NULL;
		Event->ie_Class		= IECLASS_RAWMOUSE;
		Event->ie_SubClass	= 0;
		Event->ie_Code		= IECODE_NOBUTTON;
		Event->ie_Qualifier	= IEQUALIFIER_RELATIVEMOUSE | in->Qualifiers;
		Event->ie_X			= report->X;
		Event->ie_Y			= report->Y;

		ioreq = & in->Input_IOReq;
		ioreq->io_Command	= IND_ADDEVENT;
		ioreq->io_Data		= Event;
		ioreq->io_Length	= sizeof( struct InputEvent );

		IO_DO( ioreq );
	}

	TASK_NAME_LEAVE();
}

// --

SEC_CODE static void myHID_Boot_DoMouse( struct USBBase *usbbase, struct intern *in, struct RealRequest *ioreq )
{
struct USB3_BootMouse *report;

	TASK_NAME_ENTER( "HID_Boot_Mouse : myHID_Boot_DoMouse" );

	report = (PTR) ioreq->req_Public.io_Data;

	#if 0
	USBDEBUG( "Buttons . : $%02lx", (U32) report->Buttons );
	USBDEBUG( "X ....... : %ld", (S32) report->X );
	USBDEBUG( "Y ....... : %ld", (S32) report->Y );
	#endif

	// -- Check for Button mask

	HID_Boot_Mouse_Buttons( usbbase, in, report );	

	// -- Check for Movements

	myHID_Boot_Mouse_Movement( usbbase, in, report );

	TASK_NAME_LEAVE();
}

// --

SEC_CODE void HID_Boot_Mouse( struct USBBase *usbbase, struct intern *in )
{
struct RealRequest *ioreq;
struct ExecIFace *IExec;
U32 reply;
U32 max;

	TASK_NAME_ENTER( "HID_Boot_Mouse : HID_Boot_Mouse" );

	IExec = usbbase->usb_IExec;

	max = HID_IOReqCount;

	while( TRUE )
	{
		if ( ! max-- )
		{
			break;
		}

		ioreq = (PTR) IExec->GetMsg( in->Res_Interrupt->MsgPort );

		if ( ! ioreq )
		{
			break;
		}

//		USBDEBUG( "HID_Boot_Mouse : Got IORequest %p", ioreq );

		reply = TRUE;

		switch( ioreq->req_Public.io_Error )
		{
			case USB3Err_NoError:
			{
//				USBDEBUG( "HID_Boot_Mouse : Got Int" );

				if ( ioreq->req_Public.io_Actual >= sizeof( struct USB3_BootMouse ))
				{
					in->ErrorCnt = 0;

					myHID_Boot_DoMouse( usbbase, in, ioreq );
				}
				else
				{
					USBDEBUG( "HID_Boot_Mouse : Discard (Size %ld)", ioreq->req_Public.io_Actual );
				}
				break;
			}

			case USB3Err_Device_Detached:
			{
				USBDEBUG( "HID_Boot_Mouse : Int : Detached" );
				in->Running = FALSE;
				break;
			}

			default:
			{
				in->ErrorCnt++;

				USBDEBUG( "HID_Boot_Mouse : Unknown error %ld (ErrorCnt %ld)", ioreq->req_Public.io_Error, in->ErrorCnt );

				if ( in->ErrorCnt >= 20 )
				{
					USBDEBUG( "HID_Boot_Mouse : Too many errors, quitting" );
					in->Running = FALSE;
					reply = FALSE;
				}
				break;
			}
		}

		if ( reply )
		{
//			USBDEBUG( "HID_Boot_Mouse : Reply IORequest %p", ioreq );
			IO_SEND( ioreq );
		}
	}

	TASK_NAME_LEAVE();
}

// --
