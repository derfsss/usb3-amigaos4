
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_HID_H
#define INC_PRIVATE_HID_H

/***************************************************************************/

#pragma pack(1)

/***************************************************************************/
// Qualifier keys KeyCode range start and end KeyCodes

#define USBHID_SUBCLASS_REPORT			0
#define USBHID_SUBCLASS_BOOT			1

#define USBHID_PROTOCOL_NONE			0
#define USBHID_PROTOCOL_KEYBOARD		1
#define USBHID_PROTOCOL_MOUSE			2

#define USBHID_BOOTKBD_MODF_LCTRL		0x01
#define USBHID_BOOTKBD_MODF_LSHIFT		0x02
#define USBHID_BOOTKBD_MODF_LALT		0x04
#define USBHID_BOOTKBD_MODF_LCOMMAND	0x08
#define USBHID_BOOTKBD_MODF_RSHIFT		0x20
#define USBHID_BOOTKBD_MODF_RCTRL		0x10
#define USBHID_BOOTKBD_MODF_RALT		0x40
#define USBHID_BOOTKBD_MODF_RCOMMAND	0x80

#define USBHID_BOOTKBD_LEDF_NUMLOCK		0x01
#define USBHID_BOOTKBD_LEDF_CAPSLOCK	0x02
#define USBHID_BOOTKBD_LEDF_SCROLLLOCK	0x04
#define USBHID_BOOTKBD_LEDF_COMPOSE		0x08
#define USBHID_BOOTKBD_LEDF_KANA		0x10

#define USBHID_BOOTKBD_KEY_CAPSLOCK		0x39
#define USBHID_BOOTKBD_KEY_NUMLOCK		0x53

#define USBHID_REPORTTYPE_INPUT			0x01
#define USBHID_REPORTTYPE_OUTPUT		0x02
#define USBHID_REPORTTYPE_FEATURE		0x03

#define DSCTYPE_HID						0x21
#define DSCTYPE_HID_REPORT				0x22

#define REQCODE_Get_Report				0x01
#define REQCODE_Get_Idle				0x02
#define REQCODE_Get_Protocol			0x03

#define REQCODE_Set_Report				0x09
#define REQCODE_Set_Idle				0x0A
#define REQCODE_Set_Protocol			0x0B

/***************************************************************************/

#pragma pack()

/***************************************************************************/

#endif // INC_PRIVATE_HUB_H
