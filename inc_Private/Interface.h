
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

struct USB2_InterfaceGroup
{
	struct USB2_Node				ig_Node;

	// -- 
	U32								ig_StructID;
	S32								ig_Locks;
	U16								ig_Detach;
	U16								ig_FreeMe;
//	struct USB2_TaskNode *			ig_Task; 
//	struct USB2_ASync *				ig_ASync;
	// --

	struct USB2_Header				ig_Group;
	U16								ig_Class;
	U16								ig_SubClass;
	U16								ig_Protocol;
	struct RealFunctionNode *		ig_Function;
	struct USB2_ConfigNode *		ig_Config;

	#if 0
	S32								ig_Delete;
	#endif
};

/***************************************************************************/

struct USB2_InterfaceHeader
{
	struct USB2_Node				ih_Node;

	// -- 
	U32								ih_StructID;
	S32								ih_Locks;
	U16								ih_Detach;
	U16								ih_FreeMe;
//	struct USB2_TaskNode *			ih_Task; 
//	struct USB2_ASync *				ih_ASync;
	// --

	struct USB2_Interface			ih_Public;
	U32								ih_Number;
	U32								ih_AltNumber;				// Active Alt nr
	struct USB2_Header				ih_AltSettings;
	struct USB2_InterfaceNode *		ih_Active;
	struct USB2_InterfaceGroup *	ih_Group;					// Parent
	struct RealFunctionNode *		ih_Function;
	struct RealRegister *			ih_Owner;
	USB2_ID							ih_NotifyID;

	#if 0
	S32								ih_Delete;

//	struct USB2_StartuTaskMsg		ih_StartaskMsg;
	struct USB2_FktDriverNode *		ih_FDriverNode;				// if AutoStarted via a Function Driver
	STR								ih_ProcessName;


//	U8								ih_NeedPromotion;
//	U8								ih_Pad1[3];

//	U16								ih_Pad2[1];

//	struct USB2_Node				ifc_ResourceTrackingNode;
	#endif
};

/***************************************************************************/

struct USB2_InterfaceNode
{
	struct USB2_Node				in_Node;

	// -- 
	U32								in_StructID;
	S32								in_Locks;
	U16								in_Detach;
	U16								in_FreeMe;
//	struct USB2_TaskNode *			in_Task; 
//	struct USB2_ASync *				in_ASync;
	// --

	U32								in_AltNumber;
	struct USB2_Header				in_EndPoints;
	struct USB2_Interface_Desc *	in_Descriptor;
	struct USB2_InterfaceHeader *	in_Parent;
	struct RealFunctionNode *		in_Function;
	U16 *							in_String;
	USB2_ID							in_StringID;
	S32								in_StringLen;
};

/**************************************************************************/

#endif // INC_PRIVATE_INTERFACE_H
