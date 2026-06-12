
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_ALL_H
#error Include "inc/All.h" first
#endif

#ifndef INC_USB_H
#define INC_USB_H

// --

enum
{
	TYPE_HCD,
	TYPE_FUNCTION,
	TYPE_INTERFACE,
	TYPE_LAST
};

// --

struct USB3_FktNode
{
	struct Node						Node;
	USB3_ID							ID;
	U32								Type;
	STR								Name;
	U32								Port;		// Port Nr 1-xx
	U32								Ports;		// Valid for HUB Class
	struct List						Header;

	struct USB3_FktNode **			Functions;	 // HUB Only

	char							ManufacturerStr[32+4];
	char							InterfaceStr[32+4];
	char							ProductStr[32+4];
	char							SerialStr[32+4];

	struct USB3_Config_Desc *		CfgDesc;
	S32								CfgSize;

	struct USB3_Interface_Desc *	IfcDesc;
	S32								IfcSize;

	struct USB3_Device_Desc			DevDsc;
};

// --

#endif
