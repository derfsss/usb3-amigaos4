
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef DRIVERS_HID_HID_H
#define DRIVERS_HID_HID_H

/***************************************************************************/

#define HID_IOReqCount				3
#define HID_LED_NumLock				1
#define HID_LED_CapsLock			2
#define HID_LED_ScrollLock			4
#define HID_LED_Compose				8
#define HID_LED_Kana				16

// --


enum HID_Driver_Type
{
	HID_DType_Unset,
	HID_DType_Keyboard,
	HID_DType_Mouse,
	HID_DType_Other
};

enum HID_Driver_Mode
{
	HID_DMode_Unset,
	HID_DMode_Boot,
	HID_DMode_Report
};

// --

#pragma pack(1)

struct USB3_BootMouse		// 3 bytes
{
	U8		Buttons;
	S8		X;
	S8		Y;
//	S8		Wheel;			// non standard, can really be anything
};

#define BOOTMOUSE_BUTTONS_LEFT			0x01U
#define BOOTMOUSE_BUTTONS_RIGHT			0x02U
#define BOOTMOUSE_BUTTONS_MIDDLE		0x04U
#define BOOTMOUSE_BUTTONS_4TH			0x08U
#define BOOTMOUSE_BUTTONS_5TH			0x10U
#define BOOTMOUSE_BUTTONS_MASK			0x1FU

struct USB3_BootKeyboard	// 8 bytes
{
	U8		Modifier;
	U8		Reserved;
	U8		Keycode[6];
};

#pragma pack(0)

/***************************************************************************/

struct _Boot_Keyboard
{
	struct USB3_BootKeyboard		KeyboardData;
	struct Preferences				Prefs_Buffer;
	struct TimeVal					Time_Thresh;
	struct TimeVal					Time_Repeat;
	struct TimeRequest				Timer_IOReq;
	struct USB3_MsgPort				Timer_MsgPort;
	U16		Timer_Added;
	S16		PrevCode[2];
	U16		PrevQual[2];
	U16		Key_LEDs;
	S16		Key_Last;		// We want this key to become active
	S16		Key_Repeat;		// Active key
};

struct _Boot_Mouse
{
	U8		Buttons;
};

struct KeyboardMask
{
	U8		Mask1;
	U8		Mask2;
	U8		KeyCode;
	U8		Pad;
};

struct intern
{
	#ifdef DO_DEBUG
	U32								StructID;
	#endif

	enum HID_Driver_Type			Driver_Type;
	enum HID_Driver_Mode			Driver_Mode;

	struct USB3_EPResource *		Res_Interrupt;
	struct USB3_EPResource *		Res_Control;
	struct USB3_DriverMessage *		StartMsg;
	struct RealRegister *			Register;

	struct USB3_MsgPort				Input_MsgPort;
	struct IOStdReq					Input_IOReq;
	struct InputEvent				Input_Event;

	U32		Qualifiers;
	U32		ErrorCnt;
	U32		Running;

	union
	{
		struct _Boot_Keyboard		Boot_Keyboard;
		struct _Boot_Mouse			Boot_Mouse;
	} 								Type;
};

extern const struct KeyboardMask KeyboardData[];
extern const S16 USBNumPadMap[256];
extern const S16 USBKeyMap1[256];
extern const S16 USBKeyMap2[256];

SEC_CODE void	HID_SendRawKey( struct USBBase *usbbase, struct intern *in, U16 rawkey );
SEC_CODE void	HID_Boot_Mouse_Buttons( struct USBBase *usbbase, struct intern *in, struct USB3_BootMouse *report );
SEC_CODE void	HID_Boot_Handle_Stack( struct USBBase *usbbase, struct intern *in );
SEC_CODE void	HID_Boot( struct USBBase *usbbase, struct intern *in );
SEC_CODE void	HID_Free( struct USBBase *usbbase, struct intern *in );
SEC_CODE S32	HID_Init( struct USBBase *usbbase, struct intern *in );

SEC_CODE void	HID_Boot_DoKeyboardTimer( struct USBBase *usbbase, struct intern *in );
SEC_CODE void	HID_Boot_Keyboard( struct USBBase *usbbase, struct intern *in );
SEC_CODE void	HID_Boot_Mouse( struct USBBase *usbbase, struct intern *in );

/***************************************************************************/

#endif // DRIVERS_HUB_HUB_H
