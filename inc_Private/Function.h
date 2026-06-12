
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_FUNCTION_H
#define INC_PRIVATE_FUNCTION_H

/***************************************************************************/

struct RealFunctionNode
{
	struct USB3_Function			fkt_Public;					// At the moment must be first, todo: do same as EPResource or ?
	U32								fkt_StructID;
	S32								fkt_Locks;
	U16								fkt_Detach;
	U16								fkt_FreeMe;
//	struct USB3_TaskNode *			fkt_Task; 
	struct USB3_ASync *				fkt_ASync;
	// --
	S32								fkt_ActiveIOReq;
	struct USB3_Semaphore			fkt_Semaphore;
//	struct USB3_ASync *				fkt_HUB_ASync;
	struct RealFunctionNode *		fkt_Parent;
	USB3_ID							fkt_NotifyID;
//	STR								fkt_ProcessName;

	U8								fkt_SelfPowered;
	U8								fkt_Initialized;
//	U8								fkt_NeedPromotion;
//	U8								fkt_Freeing;				// Used by Unlock function
//	U8								fkt_FreeAgain;				// Some resources have been freeing
//	U8								fkt_Pad[2];

	U32								fkt_Tier;
	U32								fkt_Address;
	U32								fkt_PortNr;
	U32								fkt_LanguageID;

	U16								fkt_Class;
	U16								fkt_SubClass;
	U16								fkt_Protocol;
//	U16								fkt_ConfigNr;

	U32								fkt_Speed;					// Max Speed

	U16 *							fkt_ManufacturerStr;
	S32								fkt_ManufacturerStrLen;
	U16 *							fkt_DeviceStr;
	S32								fkt_DeviceStrLen;
	U16 *							fkt_SerialStr;
	S32								fkt_SerialStrLen;

	U32								fkt_HUB_Data;				// Used by HUB

//	struct USB3_Header				fkt_Drivers;				// Driver connect too this Fkt, USB3_DriverNode, use dev_Process_Semaphore

	struct USB3_Node				fkt_DriverNode;				// HUB list
	struct USB3_Node				fkt_GlobalNode;				// Device list

	struct USB3_Header				fkt_Configs;
	struct USB3_ConfigNode *		fkt_Config_Active;
	U32								fkt_Config_Desc_Size;
	struct USB3_Descriptor *		fkt_Config_Desc_Buf;

	struct USB3_Device_Desc *		fkt_DeviceDescriptor;
	struct USB3_HUB_Desc *			fkt_HUBDescriptor;			// Only valid when HUB Device

	struct USB3_HCDNode *			fkt_HCD;
	struct USB3_EndPointNode *		fkt_ControlEndPoint;

//	struct USB3_FunctionDriverNode *fkt_FunctionDriverNode;		// if AutoStarted via a Function Driver

	S32								fkt_InterfaceClaimLocks;
	struct RealRegister *			fkt_Owner;
//	S32								fkt_Locks;
//	struct USB3_Header				fkt_ExpungedList;
//	struct USB3_Node				fkt_ResourceTrackingNode;
};

/***************************************************************************/

#endif // INC_PRIVATE_FUNCTION_H
