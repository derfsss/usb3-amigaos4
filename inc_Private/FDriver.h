
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_FUNCTIONDRIVER_H
#define INC_PRIVATE_FUNCTIONDRIVER_H

/***************************************************************************/

#define FDCLASS_Any			65535
#define FDSUBCLASS_Any		65535
#define FDPROTOCOL_Any		65535

/***************************************************************************/

struct USB2_FktDriverNode
{
	struct USB2_Node				fdn_Node;

	// -- 
	U32								fdn_StructID;
	S32								fdn_Locks;
	U16								fdn_Detach;
	U16								fdn_FreeMe;
//	struct USB2_TaskNode *			fdn_Task;
//	struct USB2_ASync *				fdn_ASync;
	// --

	U32								fdn_Type;
	S32								fdn_Pri;
	// --
	U16								fdn_Class;
	U16								fdn_SubClass;
	U16								fdn_Protocol;
	// --
	struct List						fdn_Devices;		// Exec List, don't change
	U32							( * fdn_Entry )  ( struct USBBase *usbbase, struct USB2_DriverMessage *msg );


//  STR								fdn_Self;				// own FileName
	STR								fdn_Title;
	STR								fdn_Driver_Filename;	// FileName of the Driver
	STR								fdn_Class_Filename;		// FileName of the Class info
	U32								fdn_Class_FilenameLen;
//	STR								fdn_Hotkey;
	struct DateStamp				fdn_ClassDate;
	// --
//	uint16							fdn_Pad1;
//	uint16							fdn_Pad2;
//	uint16							fdn_Pad3;
//	// --
//	U32								fdn_StackSize;
//	void						( * fdn_Promote );
//	#if 0
//	struct NotifyRequest *			fdn_NotifyReq;
//	#endif
//	uint16							fdn_DevicesPad;
};

/***************************************************************************/

#endif // INC_PRIVATE_FUNCTIONDRIVER_H
