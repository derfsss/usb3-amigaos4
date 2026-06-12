
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_INTERFACE_H
#define INC_PRIVATE_INTERFACE_H

/***************************************************************************/
// I think, this is my 'USB interface association descriptor'
// code cfg parse.. need update
// Interfaces without a IAD, will get one dummy anyway

struct USB3_InterfaceGroup
{
	struct USB3_Node				ig_Node;

	// -- 
	U32								ig_StructID;
	S32								ig_Locks;
	U16								ig_Detach;
	U16								ig_FreeMe;
//	struct USB3_TaskNode *			ig_Task; 
//	struct USB3_ASync *				ig_ASync;
	// --

	struct USB3_Header				ig_Group;
	U16								ig_Class;
	U16								ig_SubClass;
	U16								ig_Protocol;
	struct RealFunctionNode *		ig_Function;
	struct USB3_ConfigNode *		ig_Config;

	#if 0
	S32								ig_Delete;
	#endif
};

/***************************************************************************/

struct USB3_InterfaceHeader
{
	struct USB3_Node				ih_Node;

	// -- 
	U32								ih_StructID;
	S32								ih_Locks;
	U16								ih_Detach;
	U16								ih_FreeMe;
//	struct USB3_TaskNode *			ih_Task; 
//	struct USB3_ASync *				ih_ASync;
	// --

	struct USB3_Interface			ih_Public;
	U32								ih_Number;
	U32								ih_AltNumber;				// Active Alt nr
	struct USB3_Header				ih_AltSettings;
	struct USB3_InterfaceNode *		ih_Active;
	struct USB3_InterfaceGroup *	ih_Group;					// Parent
	struct RealFunctionNode *		ih_Function;
	struct RealRegister *			ih_Owner;
	USB3_ID							ih_NotifyID;

	#if 0
	S32								ih_Delete;

//	struct USB3_StartuTaskMsg		ih_StartaskMsg;
	struct USB3_FktDriverNode *		ih_FDriverNode;				// if AutoStarted via a Function Driver
	STR								ih_ProcessName;


//	U8								ih_NeedPromotion;
//	U8								ih_Pad1[3];

//	U16								ih_Pad2[1];

//	struct USB3_Node				ifc_ResourceTrackingNode;
	#endif
};

/***************************************************************************/

struct USB3_InterfaceNode
{
	struct USB3_Node				in_Node;

	// -- 
	U32								in_StructID;
	S32								in_Locks;
	U16								in_Detach;
	U16								in_FreeMe;
//	struct USB3_TaskNode *			in_Task; 
//	struct USB3_ASync *				in_ASync;
	// --

	U32								in_AltNumber;
	struct USB3_Header				in_EndPoints;
	struct USB3_Interface_Desc *	in_Descriptor;
	struct USB3_InterfaceHeader *	in_Parent;
	struct RealFunctionNode *		in_Function;
	U16 *							in_String;
	USB3_ID							in_StringID;
	S32								in_StringLen;
};

/**************************************************************************/

#endif // INC_PRIVATE_INTERFACE_H
