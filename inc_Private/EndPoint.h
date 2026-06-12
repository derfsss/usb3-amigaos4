
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_ENDPOINT_H
#define INC_PRIVATE_ENDPOINT_H

/***************************************************************************/

#define	EPDIR_In		1		// Bit %01, read
#define EPDIR_Out		2		// Bit %10, write
#define EPDIR_Both		3		// Bit %11, read+write

/***************************************************************************/

struct USB2_EndPointNode
{
	struct USB2_Node				ep_Node;

	// -- 
	U32								ep_StructID;
	S32								ep_Locks;
	U16								ep_Detach;
	U16								ep_FreeMe;
//	struct USB2_TaskNode *			ep_Task; 
//	struct USB2_ASync *				ep_ASync;
	// --

	struct RealFunctionNode *		ep_Function;
	struct USB2_InterfaceNode *		ep_Interface;
	struct USB2_EndPoint_Desc *		ep_Descriptor;
	struct RealRequest *			ep_Request_Active;
	struct USB2_Header				ep_Request_Queue;

	U32								ep_Type;				// EPATASK_Type_Type_xxx
	U32								ep_Number;				// Number 0-15
	U32								ep_Direction;			// EPDir_xxx
	U32								ep_Interval;			// 0-255 ms
	U32								ep_MaxPacketSize;
	U32								ep_Stalled;				// EndPoint is stalled, need Destall (TRUE/FALSE)
	U32								ep_DataToggle;
	U32								ep_Active;
	U32								ep_PingState;			// EHCI Flag
//	U32								ep_Pad[3];
};

struct RealEndPointResource
{
	struct USB2_Node				epr_Node;
	U32								epr_StructID;
	S32								epr_Locks;
	U16								epr_Detach;
	U16								epr_FreeMe; 
	struct USB2_EPResource			epr_Public;
 	struct RealRegister *			epr_Register;

//	S32								epr_Delete;
//	struct MsgPort					epr_MsgPort;
//	struct USB2_MsgPort				epr_MsgPort;
//	int								epr_MsgPortUsed;
};

/***************************************************************************/

#endif // INC_PRIVATE_ENDPOINT_H
