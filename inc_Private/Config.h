
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_CONFIG_H
#define INC_PRIVATE_CONFIG_H

/***************************************************************************/

struct USB2_ConfigNode
{
	struct USB2_Node				cfg_Node;
	// -- 
	U32								cfg_StructID;
	S32								cfg_Locks;
	U16								cfg_Detach;
	U16								cfg_FreeMe;
//	struct USB2_TaskNode *			cfg_Task;
//	struct USB2_ASync *				cfg_ASync;
	// --
	U32								cfg_Number;
	struct RealFunctionNode *		cfg_Function;
	struct USB2_Config_Desc *		cfg_Descriptor;
	struct USB2_Header				cfg_InterfaceGroups;
	U16 *							cfg_String;
	S32								cfg_StringLen;
	USB2_ID							cfg_StringID;
};

/***************************************************************************/

#endif // INC_PRIVATE_CONFIG_H
