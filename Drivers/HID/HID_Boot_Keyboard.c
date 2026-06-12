
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
// Send Amiga Raw Key to System

SEC_CODE void HID_SendRawKey( struct USBBase *usbbase, struct intern *in, U16 rawkey )
{
struct InputEvent *Event;
struct IOStdReq *ioreq;
U16 key;

	// --

	TASK_NAME_ENTER( "HID_Boot_Keyboard : HID_SendRawKey" );

	// --

	Event = & in->Input_Event;
	Event->ie_NextEvent		= NULL;
	Event->ie_Class			= IECLASS_RAWKEY;
	Event->ie_SubClass		= 0;
	Event->ie_Code			= rawkey;
	Event->ie_Qualifier		= in->Qualifiers;
	Event->ie_Prev1DownCode	= in->Type.Boot_Keyboard.PrevCode[0];
	Event->ie_Prev1DownQual	= in->Type.Boot_Keyboard.PrevQual[0];
	Event->ie_Prev2DownCode	= in->Type.Boot_Keyboard.PrevCode[1];
	Event->ie_Prev2DownQual	= in->Type.Boot_Keyboard.PrevQual[1];

	ioreq = & in->Input_IOReq;
	ioreq->io_Command		= IND_ADDEVENT;
	ioreq->io_Data			= Event;
	ioreq->io_Length		= sizeof( struct InputEvent );

	IO_DO( ioreq );

	// --
	// Amiga Raw KeyCodes
	//  $60 = LShift
	//  $61 = RShift
	//  $62 = Caps Lock
	//  $63 = Ctrl
	//  $64 = LAlt
	//  $65 = RAlt
	//  $66 = LAmiga / LCommand
	//  $67 = RAmiga / RCommand
	//	$68 = Left Mouse button (not used)
	//	$69 = Right Mouse button (not used)
	//	$6A = Middle Mouse button (not used)
	key = ( rawkey & ~IECODE_UP_PREFIX );

	if (( key < 0x60 ) && ( key > 0x6A ))
	{
		in->Type.Boot_Keyboard.PrevCode[1] = in->Type.Boot_Keyboard.PrevCode[0];
		in->Type.Boot_Keyboard.PrevCode[0] = rawkey;
		in->Type.Boot_Keyboard.PrevQual[1] = in->Type.Boot_Keyboard.PrevQual[0];
		in->Type.Boot_Keyboard.PrevQual[0] = in->Qualifiers;
	}

	// --

	TASK_NAME_LEAVE();
}

// --

SEC_CODE void HID_SendLED( struct USBBase *usbbase, struct intern *in )
{
struct USB2_IORequest *ioreq;
struct USB2_SetupData *sd;
U16 ifcnr;
U8 leds;

	// --

	TASK_NAME_ENTER( "HID_SendLED : HID_SendLED" );

	// --

	leds	= in->Type.Boot_Keyboard.Key_LEDs;
	ifcnr	= in->StartMsg->InterfaceDescriptor->InterfaceNumber;

	sd = in->Res_Control->SetupData;
	sd->RequestType			= REQTYPE_Write | REQTYPE_Class | REQTYPE_Interface;
	sd->RequestCode			= REQCODE_Set_Report;
	sd->Value				= LE_SWAP16( 0x0200 );	// Output Report | ID 0
	sd->Index				= LE_SWAP16( ifcnr );
	sd->Length				= LE_SWAP16( 1 );

	ioreq = in->Res_Control->IORequests[0];
	ioreq->io_Command		= CMD_WRITE;
	ioreq->io_Data			= & leds;
	ioreq->io_Length		= 1;
	ioreq->io_SetupData		= sd;
	ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

	IO_DO( ioreq );
	
	// --

	TASK_NAME_LEAVE();
}

// --
// Check if Keyboard buffer triggered a rollover

SEC_CODE static S32 myHID_Boot_Kbd_RollOver( struct USB2_BootKeyboard *report )
{
S32 retval;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : myHID_Boot_Kbd_RollOver" );

	retval = ( report->Keycode[0] == 0x01 ) ? TRUE : FALSE ;

	TASK_NAME_LEAVE();

	return( retval );
}

// --
// Check for changed Modifier

SEC_CODE static void myHID_Boot_Kbd_Modifiers( struct USBBase *usbbase, struct intern *in, U8 newmod )
{
U32 pos;
U16 key;
U8 mask1;
U8 mask2;

	pos = 0;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : myHID_Boot_Kbd_Modifiers" );

	while( TRUE )
	{
		if ( ! KeyboardData[pos].Mask1 )
		{
			break;
		}

		mask1 = ( KeyboardData[pos].Mask1 & newmod			) ? 1 : 0;
		mask2 = ( KeyboardData[pos].Mask2 & in->Qualifiers	) ? 1 : 0;

		if ( mask1 != mask2 )
		{
			if ( mask1 )
			{
				key = KeyboardData[pos].KeyCode;
				in->Qualifiers |= KeyboardData[pos].Mask2;
			}
			else
			{
				key = KeyboardData[pos].KeyCode | IECODE_UP_PREFIX;
				in->Qualifiers &= ~KeyboardData[pos].Mask2;
			}

			HID_SendRawKey( usbbase, in, (U16) key );
		}

		pos++;
	}

	// -- Check for Reboot qualifiers

	if ((( IEQUALIFIER_CONTROL | IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND ) & in->Qualifiers )
	==	(( IEQUALIFIER_CONTROL | IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND )))
	{
		MISC_COLDREBOOT();
	}

	if ((( IEQUALIFIER_CONTROL | IEQUALIFIER_LALT | IEQUALIFIER_RALT ) & in->Qualifiers )
	==	(( IEQUALIFIER_CONTROL | IEQUALIFIER_LALT | IEQUALIFIER_RALT )))
	{
		MISC_ICECOLDREBOOT();
	}

	TASK_NAME_LEAVE();
}

// --
// Check for Key Press

SEC_CODE static void myHID_Boot_Kbd_KeyPress( 
	struct USBBase *usbbase, 
	struct intern *in, 
	struct USB2_BootKeyboard *oldreport,
	struct USB2_BootKeyboard *newreport )
{
S16 amikey;
S16 usbkey;
U32 newpos;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : myHID_Boot_Kbd_KeyPress" );

	for( newpos=0 ; newpos<6 ; newpos++ )
	{
		usbkey = newreport->Keycode[newpos];

		// 0 = SKip, 1 = Roll Over, 2+ = Valid Key
		if ( usbkey < 2 )
		{
			continue;
		}

		// Now check if its a new one
		if (( oldreport->Keycode[0] == usbkey )
		||	( oldreport->Keycode[1] == usbkey )
		||	( oldreport->Keycode[2] == usbkey )
		||	( oldreport->Keycode[3] == usbkey )
		||	( oldreport->Keycode[4] == usbkey )
		||	( oldreport->Keycode[5] == usbkey ))
		{
			// No we have handled this one before
			continue;
		}
		
		// --
		// not sure this is the best place to handle 'Locks'
	
		if ( usbkey == 57 )
		{
			// Handle CapsLock... 
			in->Qualifiers ^= IEQUALIFIER_CAPSLOCK;
			in->Type.Boot_Keyboard.Key_LEDs ^= HID_LED_CapsLock;
			HID_SendLED( usbbase, in );
		}
	
		if ( usbkey == 71 )
		{
			// Handle ScrollLock
			in->Type.Boot_Keyboard.Key_LEDs ^= HID_LED_ScrollLock;
			HID_SendLED( usbbase, in );
		}

		if ( usbkey == 83 )
		{
			// Handle NumLock
			in->Qualifiers ^= IEQUALIFIER_NUMERICPAD;
			in->Type.Boot_Keyboard.Key_LEDs ^= HID_LED_NumLock;
			HID_SendLED( usbbase, in );
		}

		// Conver USB Keycode to Amiga Keycode
		amikey = ( in->Qualifiers & IEQUALIFIER_NUMERICPAD ) ? USBKeyMap1[ usbkey ] : USBKeyMap2[ usbkey ] ;

		// a Nagative, mean key is not mapped
		if ( amikey < 0 )
		{
			continue;
		}

		// Remeber Last key for Repeat
		in->Type.Boot_Keyboard.Key_Last = usbkey;

		HID_SendRawKey( usbbase, in, (U16) amikey );
	}

	TASK_NAME_LEAVE();
}

// --
// Check for Key Releases

SEC_CODE static void myHID_Boot_Kbd_KeyRelease( 
	struct USBBase *usbbase, 
	struct intern *in, 
	struct USB2_BootKeyboard *oldreport,
	struct USB2_BootKeyboard *newreport )
{
U32 oldpos;
S16 amikey;
S16 usbkey;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : myHID_Boot_Kbd_KeyRelease" );

	for( oldpos=0 ; oldpos<6 ; oldpos++ )
	{
		usbkey = oldreport->Keycode[oldpos];

		// 0 = Skip, 1 = Roll Over, 2+ = Valid Key
		if ( usbkey < 2 )
		{
			continue;
		}

		// Now check if its a still there
		if (( newreport->Keycode[0] == usbkey )
		||	( newreport->Keycode[1] == usbkey )
		||	( newreport->Keycode[2] == usbkey )
		||	( newreport->Keycode[3] == usbkey )
		||	( newreport->Keycode[4] == usbkey )
		||	( newreport->Keycode[5] == usbkey ))
		{
			// key still pressed
			continue;
		}

		// Check if the released key is the repeating one
		if ( in->Type.Boot_Keyboard.Key_Repeat == usbkey )
		{
			// Stop current repeat key,
			// another key may start repearting (the last one)
			in->Type.Boot_Keyboard.Key_Last = 0;
		}

		// Map key press to Amiga raw key code
		amikey = ( in->Qualifiers & IEQUALIFIER_NUMERICPAD ) ? USBKeyMap1[ usbkey ] : USBKeyMap2[ usbkey ] ;

		// a Nagative, mean key is not mapped
		if ( amikey < 0 )
		{
			continue;
		}

		HID_SendRawKey( usbbase, in, (U16) amikey | IECODE_UP_PREFIX );
	}

	TASK_NAME_LEAVE();
}

// --
// Handle Key Repeat

SEC_CODE static void myHID_Boot_Kbd_KeyRepeat( 
	struct USBBase *usbbase, 
	struct intern *in )
{
S16 repeat;
S16 last;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : myHID_Boot_Kbd_KeyRepeat" );

	// Note:
	//  Last - We want to make this one active .. if not clear
	//  repeat - this is the active key
	last	= in->Type.Boot_Keyboard.Key_Last;
	repeat	= in->Type.Boot_Keyboard.Key_Repeat;

//	USBDEBUG( "Last: $%04lx, Repeat: $%04lx", (U32) last, (U32) repeat );

	if ( last != repeat )
	{
		// Start or Change Key Repeat
		in->Type.Boot_Keyboard.Key_Repeat = last;

		// Stop timer, if active
		if ( in->Type.Boot_Keyboard.Timer_Added )
		{
			in->Type.Boot_Keyboard.Timer_Added = FALSE;
			// Stop Timer
			IO_STOP( & in->Type.Boot_Keyboard.Timer_IOReq );
			// Clear Timer signal, avoid triggering a repeat
			TASK_SETSIGNAL( 0, in->Type.Boot_Keyboard.Timer_MsgPort.ump_Signal.sig_Signal_Mask );
		}

		if ( last )
		{
			// We only need the first 20 bytes to get Delay and Repeat times
			if ( MISC_GETINTUIPREFS( & in->Type.Boot_Keyboard.Prefs_Buffer, 20 ))
			{
				in->Type.Boot_Keyboard.Time_Thresh = in->Type.Boot_Keyboard.Prefs_Buffer.KeyRptDelay;
				in->Type.Boot_Keyboard.Time_Repeat = in->Type.Boot_Keyboard.Prefs_Buffer.KeyRptSpeed;
			}

			// Start Timer
			if (( in->Type.Boot_Keyboard.Time_Thresh.Seconds )
			||	( in->Type.Boot_Keyboard.Time_Thresh.Microseconds ))
			{
				in->Type.Boot_Keyboard.Timer_IOReq.Time = in->Type.Boot_Keyboard.Time_Thresh;
				in->Type.Boot_Keyboard.Timer_IOReq.Request.io_Command = TR_ADDREQUEST;
				in->Type.Boot_Keyboard.Timer_Added = TRUE;
				IO_SEND( & in->Type.Boot_Keyboard.Timer_IOReq );
			}
			else
			{
				USBDEBUG( "myHID_Boot_Kbd_KeyRepeat : Time_Thresh time is Zero" );
			}
		}
	}

	TASK_NAME_LEAVE();
}

// --

SEC_CODE static void myHID_Boot_DoKeyboard( struct USBBase *usbbase, struct intern *in, struct RealRequest *ioreq )
{
struct USB2_BootKeyboard *newreport;
struct USB2_BootKeyboard *oldreport;

	newreport = (PTR) ioreq->req_Public.io_Data;
	oldreport = & in->Type.Boot_Keyboard.KeyboardData;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : myHID_Boot_DoKeyboard" );

	#if 0
	USBDEBUG( "Last ... : $%04lx", (U32) in->Type.Boot_Keyboard.Key_Last );
	USBDEBUG( "Repeat . : $%04lx", (U32) in->Type.Boot_Keyboard.Key_Repeat );
	USBDEBUG( "HIDKBD . : New : %02lx %02lx %02lx %02lx %02lx %02lx",
		(U32) newreport->Keycode[0],
		(U32) newreport->Keycode[1],
		(U32) newreport->Keycode[2],
		(U32) newreport->Keycode[3],
		(U32) newreport->Keycode[4],
		(U32) newreport->Keycode[5]
	);

	USBDEBUG( "HIDKBD . : Old : %02lx %02lx %02lx %02lx %02lx %02lx",
		(U32) oldreport->Keycode[0],
		(U32) oldreport->Keycode[1],
		(U32) oldreport->Keycode[2],
		(U32) oldreport->Keycode[3],
		(U32) oldreport->Keycode[4],
		(U32) oldreport->Keycode[5]
	);
	#endif

	// -- Check for RollOver

	if ( myHID_Boot_Kbd_RollOver( newreport ))
	{
		USBDEBUG( "myHID_Boot_DoKeyboard : RollOver" );
		return;
	}

	// -- Process Modifiers

	if ( oldreport->Modifier != newreport->Modifier )
	{
		myHID_Boot_Kbd_Modifiers( usbbase, in, newreport->Modifier );
	}

	// -- Check for key presses

	myHID_Boot_Kbd_KeyPress( usbbase, in, oldreport, newreport );	

	// -- Check for key releases

	myHID_Boot_Kbd_KeyRelease( usbbase, in, oldreport, newreport );	

	// -- Setup key-repeat timer and related state info

	myHID_Boot_Kbd_KeyRepeat( usbbase, in );

	// -- And now copy Report for next time

	MEM_COPY( newreport, oldreport, sizeof( struct USB2_BootKeyboard ));

	TASK_NAME_LEAVE();
}

// --

SEC_CODE void HID_Boot_DoKeyboardTimer( struct USBBase *usbbase, struct intern *in )
{
S16 repeat;
S16 amikey;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : HID_Boot_DoKeyboardTimer" );

	// 0 = None, 1 = Rollover, 2+ Good
	repeat = in->Type.Boot_Keyboard.Key_Repeat;

	// 0 = Skip, 1 = Roll Over, 2+ = Valid Key
	if ( repeat < 2 )
	{
		goto bailout;
	}

	// -1 = Key not Mapped
	amikey = ( in->Qualifiers & IEQUALIFIER_NUMERICPAD ) ? USBKeyMap1[ repeat ] : USBKeyMap2[ repeat ] ;

	// a Nagative, mean key is not mapped
	if ( amikey < 0 )
	{
		goto bailout;
	}

	// Start Repeat Timer
	if (( in->Type.Boot_Keyboard.Time_Repeat.Seconds )
	||	( in->Type.Boot_Keyboard.Time_Repeat.Microseconds ))
	{
		in->Type.Boot_Keyboard.Timer_IOReq.Time = in->Type.Boot_Keyboard.Time_Repeat;
		in->Type.Boot_Keyboard.Timer_IOReq.Request.io_Command = TR_ADDREQUEST;
		in->Type.Boot_Keyboard.Timer_Added = TRUE;
		IO_SEND( & in->Type.Boot_Keyboard.Timer_IOReq );
	}
	else
	{
		USBDEBUG( "HID_Boot_DoKeyboardTimer : Time_Repeat time is Zero" );
	}

	// Insert repeat key

	in->Qualifiers |= IEQUALIFIER_REPEAT;

	HID_SendRawKey( usbbase, in, (U16) amikey );

	in->Qualifiers &= ~IEQUALIFIER_REPEAT;

bailout:

	TASK_NAME_LEAVE();

	return;
}

// --

SEC_CODE void HID_Boot_Keyboard( struct USBBase *usbbase, struct intern *in )
{
struct RealRequest *ioreq;
struct ExecIFace *IExec;
U32 reply;
U32 max;

	TASK_NAME_ENTER( "HID_Boot_Keyboard : HID_Boot_Keyboard" );

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

//		USBDEBUG( "HID_Boot_Keyboard : Got IORequest %p", ioreq );

		reply = TRUE;

		switch( ioreq->req_Public.io_Error )
		{
			case USB2Err_NoError:
			{
//				USBDEBUG( "HID_Boot_Keyboard : Got Int" );

				if ( ioreq->req_Public.io_Actual >= sizeof( struct USB2_BootKeyboard ))
				{
					in->ErrorCnt = 0;

					myHID_Boot_DoKeyboard( usbbase, in, ioreq );
				}
				else
				{
					USBDEBUG( "HID_Boot_Keyboard : Discard (Size %ld)", ioreq->req_Public.io_Actual );
				}
				break;
			}

			case USB2Err_Device_Detached:
			{
				USBDEBUG( "HID_Boot_Keyboard : Int : Detached" );
				in->Running = FALSE;
				break;
			}

			default:
			{
				in->ErrorCnt++;

//				USBDEBUG( "HID_Boot_Keyboard : Unknown error %ld (ErrorCnt %ld)", ioreq->req_Public.io_Error, in->ErrorCnt );

				if ( in->ErrorCnt >= 20 )
				{
					USBDEBUG( "HID_Boot_Keyboard : Too many errors, quitting" );
					in->Running = FALSE;
					reply = FALSE;
				}
				break;
			}
		}

		if ( reply )
		{
//			USBDEBUG( "HID_Boot_Keyboard : Reply IORequest %p", ioreq );
			IO_SEND( ioreq );
		}
	}

	TASK_NAME_LEAVE();
}

// --
