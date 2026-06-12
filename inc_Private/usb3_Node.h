
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_USB3_NODE_H
#define INC_PRIVATE_USB3_NODE_H

/***************************************************************************/

#define Header_Head(x)	((struct USB2_Header *)x)->uh_Head
//#define Header_Tail(x)	((struct USB2_Header *)x)->uh_Tail
//#define Node_ID(x)	((struct USB2_Node *)x)->un_StructID

#if 0
//#ifdef DO_PANIC

struct USBBase;
PTR __Node_Next( struct USBBase *, PTR cur, STR file );
PTR __Node_Prev( struct USBBase *, PTR cur, STR file );

#define Node_Next(x)	__Node_Next(usbbase,x,__FILE__)
#define Node_Prev(x)	__Node_Prev(usbbase,x,__FILE__)

#else

#define Node_Next(x)	((struct USB2_Node *)x)->un_Next
#define Node_Prev(x)	((struct USB2_Node *)x)->un_Prev

#endif

/***************************************************************************/

struct USB2_Header
{
	PTR							uh_Head;
	PTR							uh_Tail;
	S32							uh_Nodes;
};

struct USB2_Node
{
	PTR							un_Next;
	PTR							un_Prev;
	PTR							un_Data;
//	U32							un_StructID;
};

/***************************************************************************/

#endif // INC_PRIVATE_USB3_NODE_H
