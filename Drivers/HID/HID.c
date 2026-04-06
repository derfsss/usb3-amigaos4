
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"
#include "HID.h"

// --

SEC_RODATA const struct KeyboardMask KeyboardData[] =
{
{	USBHID_BOOTKBD_MODF_LSHIFT,		IEQUALIFIER_LSHIFT,		0x60,	0 },
{	USBHID_BOOTKBD_MODF_RSHIFT,		IEQUALIFIER_RSHIFT,		0x61,	0 },
{	USBHID_BOOTKBD_MODF_LCTRL |
	USBHID_BOOTKBD_MODF_RCTRL,		IEQUALIFIER_CONTROL,	0x63, 	0 },
{	USBHID_BOOTKBD_MODF_LALT,		IEQUALIFIER_LALT,		0x64,	0 },
{	USBHID_BOOTKBD_MODF_RALT,		IEQUALIFIER_RALT,		0x65,	0 },
{	USBHID_BOOTKBD_MODF_LCOMMAND,	IEQUALIFIER_LCOMMAND,	0x66,	0 },
{	USBHID_BOOTKBD_MODF_RCOMMAND,	IEQUALIFIER_RCOMMAND,	0x67,	0 },
{	0,								0,						0,		0 },
};

SEC_RODATA const S16 USBKeyMap1[256] = // with NumPad off
{
//    0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	 -1 , -1 , -1 , -1 ,0x20,0x35,0x33,0x22,0x12,0x23,0x24,0x25,0x17,0x26,0x27,0x28, // 0
	0x37,0x36,0x18,0x19,0x10,0x13,0x21,0x14,0x16,0x34,0x11,0x32,0x15,0x31,0x01,0x02, // 1
	0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x44,0x45,0x41,0x42,0x40,0x0b,0x0c,0x1a, // 2
	0x1b,0x0d,0x2b,0x29,0x2a,0x00,0x38,0x39,0x3a,0x62,0x50,0x51,0x52,0x53,0x54,0x55, // 3
	0x56,0x57,0x58,0x59,0x4b,0x6f,0x6d,0x5f,0x6e,0x47,0x70,0x48,0x46,0x71,0x49,0x4e, // 4
	0x4f,0x4d,0x4c,0x5a,0x5c,0x5d,0x4a,0x5e,0x43,0x1d,0x1e,0x1f,0x2d,0x2e,0x2f,0x3d, // 5
	0x3e,0x3f,0x0f,0x3c,0x30,0x6b, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // 6
	 -1,  -1,  -1,  -1,  -1, 0x5f, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // 7
	 -1,  -1,  -1,  -1,  -1, 0x6c, -1, 0x3b, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // 8
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 0x6d, -1,  -1,  -1,  -1,  -1,  // 9
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // a
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // b
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // c
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // d
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // e
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // f
};

SEC_RODATA const S16 USBKeyMap2[256] = // with NumPad on
{
//    0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	 -1 , -1 , -1 , -1 ,0x20,0x35,0x33,0x22,0x12,0x23,0x24,0x25,0x17,0x26,0x27,0x28, // 0
	0x37,0x36,0x18,0x19,0x10,0x13,0x21,0x14,0x16,0x34,0x11,0x32,0x15,0x31,0x01,0x02, // 1
	0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x44,0x45,0x41,0x42,0x40,0x0b,0x0c,0x1a, // 2
	0x1b,0x0d,0x2b,0x29,0x2a,0x00,0x38,0x39,0x3a,0x62,0x50,0x51,0x52,0x53,0x54,0x55, // 3
	0x56,0x57,0x58,0x59,0x4b,0x6f,0x6d,0x5f,0x6e,0x47,0x70,0x48,0x46,0x71,0x49,0x4e, // 4
	0x4f,0x4d,0x4c,0x5a,0x5c,0x5d,0x4a,0x5e,0x43,0x71,0x4d,0x49,0x4f,0x2e,0x4e,0x7d, // 5
	0x4c,0x48,0x47,0x46,0x30,0x6b, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // 6
	 -1,  -1,  -1,  -1,  -1 ,0x5f, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // 7
	 -1,  -1,  -1,  -1,  -1, 0x6c, -1, 0x3b, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // 8
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 0x6d, -1,  -1,  -1,  -1,  -1,  // 9
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // a
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // b
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // c
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // d
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // e
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  // f
};

SEC_RODATA const S16 USBNumPadMap[256] =
{
//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 1
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 2
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 3
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 4
	0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 5
	1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 6
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 7
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 8
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // a
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // b
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // c
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // d
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // e
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // f
};

// --

SEC_CODE U32 HID_Entry( struct USBBase *usbbase, struct USB2_DriverMessage *msg )
{
struct intern *in;
U32 retval;

	// --

	TASK_NAME_ENTER( "HID_Entry" );

	usbbase->usb_IExec->DebugPrintF( "USB: HID_Entry: msg=%p\n", msg );

	if ( ! msg )
	{
		usbbase->usb_IExec->DebugPrintF( "USB: HID_Entry: msg is NULL!\n" );
		return( TASK_Return_Error );
	}

	usbbase->usb_IExec->DebugPrintF( "USB: HID_Entry: msg->Function=%p msg->Interface=%p msg->IUSB2=%p\n",
		msg->Function, msg->Interface, msg->IUSB2 );

	if ( ! msg->Function )
	{
		usbbase->usb_IExec->DebugPrintF( "USB: HID_Entry: Function is NULL!\n" );
		return( TASK_Return_Error );
	}

	struct RealFunctionNode *fn = (PTR) msg->Function;

	usbbase->usb_IExec->DebugPrintF( "USB: HID_Entry: fn=%p addr=%ld tier=%ld\n",
		fn, (U32) fn->fkt_Address, (U32) fn->fkt_Tier );

	// --

	retval = TASK_Return_Error;

	#ifdef DO_INFO
	struct ExecIFace *IExec = usbbase->usb_IExec;
	IExec->Disable();
	IExec->DebugPrintF( "\nStartup new HID\n" );
	IExec->DebugPrintF( "HCD Index   : %ld\n", fn->fkt_HCD->hn_HCDIndex );
	IExec->DebugPrintF( "Tier        : %ld\n", fn->fkt_Tier );
	IExec->DebugPrintF( "Addr Nr     : %ld\n", fn->fkt_Address );
	IExec->DebugPrintF( "Port Nr     : %ld\n\n", fn->fkt_PortNr );
	IExec->Enable();
	#endif

	// --

	usbbase->usb_IExec->DebugPrintF( "USB: HID_Entry: allocating intern\n" );

	in = MEM_ALLOCVEC( sizeof( struct intern ), TRUE );

	if ( in )
	{
		#ifdef DO_DEBUG
		in->StructID = ID_IN_HID;
		#endif
		in->StartMsg = msg;

		usbbase->usb_IExec->DebugPrintF( "USB: HID_Entry: calling HID_Init\n" );

		if ( HID_Init( usbbase, in ))
		{
			/**/ if ( in->Driver_Mode == HID_DMode_Boot )
			{
				HID_Boot( usbbase, in );
			}
//			else if ( in->Driver_Mode == HID_DMode_Report )
//			{
//				myHID_Report( usbbase, in );
//			}
			else
			{
				USBDEBUG( "HID_Entry : Invalid Mode %ld", in->Driver_Mode );
			}
		}
		else
		{
			USBDEBUG( "HID_Entry : Error init failed" );
		}

		HID_Free( usbbase, in );
		MEM_FREEVEC( in );
	}
	else
	{
		USBDEBUG( "HID_Entry : Error allocating memory" );
	}

	// --

	#ifdef DO_DEBUG
	usbbase->usb_IExec->DebugPrintF( "\n##\n## HID_Entry : Adr #%lu (%lu) : FN    %p : Locks %2ld : Leave\n##\n\n", fn->fkt_Address, startadr, fn, fn->fkt_Locks );
	#endif

	TASK_NAME_LEAVE();

	return( retval );
}

// --
