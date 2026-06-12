
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_DRIVER_H
#define INC_PRIVATE_SRIVER_H

/***************************************************************************/

enum DriverType
{
	DT_Unset,		// hmm error I guess
	DT_HCD,			// HCD
	DT_HUB,			// Root HUB
	DT_DEV,			// Bind Device
	DT_FKT,			// Bind Function
	DT_IFC,			// Bind Interface
	DT_Last
};

struct RealDriverMessage
{
	struct USB2_DriverMessage		rdm_Public;					// At the moment must be first, todo: do same as EPResource or ?
	enum TaskReturn					rdm_Result;
	struct Task *					rdm_Parent;
};

struct USB2_DriverNode
{
	struct USB2_Node				dn_Node;

	// -- 
	U32								dn_StructID;
	// dn_StructID :: Should be ID_USB2_DN
	S32								dn_Locks;
	// dn_Locks :: Anyone that Points to this struct should lock it
	U16								dn_Detach;
	//
	U16								dn_FreeMe;
	// dn_Detach :: if Set Stop Driver
	// USB Device Detached, Driver Stopped or User want to stop
	struct USB2_TaskNode *			dn_Task;
	// dn_Task :: When a Driver Task get started via Bind
//	struct USB2_ASync *				dn_ASync;
	// dn_ASync :: Not needed for this Strict
	// --

	STR								dn_Filename;		// FileName of the Driver
	struct USB2_HCDNode *			dn_HCD;				// Should always be valid
	struct RealFunctionNode *		dn_Function;		// Valid for Dev/Fkt/Ifc not HCDs
	struct RealDriverMessage		dn_Message;			// Public .. not used for HCDs
	U32							( * dn_Entry )  ( struct USBBase *usbbase, struct USB2_DriverMessage *msg );
};

#if 0
struct USB2_DriverMessage
{
	U16								DeviceID;
	U16								DeviceID;
	U32								DriverQuirks;
	struct USB2_Function *			Function;
	struct USB2_Interface *			Interface;
	struct USB2_Descriptor *		ConfigDescriptors;
	struct USB2_Interface_Desc *	InterfaceDescriptor;
};
#endif

/***************************************************************************/

#endif // INC_PRIVATE_SRIVER_H
