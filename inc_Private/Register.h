
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_REGISTER_H
#define INC_PRIVATE_REGISTER_H

/***************************************************************************/

struct RealRegister
{
	struct USB2_Register			reg_Public;					// At the moment must be first, todo: do same as EPResource or ?

	// -- 
	U32								reg_StructID;
	S32								reg_Locks;
	U16								reg_Detach;
	U16								reg_FreeMe;
//	struct USB2_TaskNode *			reg_Task; 
	struct USB2_ASync *				reg_ASync;
	// --

	STR								reg_Title;
//	struct USB2_EPResource *	reg_Control;
	struct USB2_MsgPort				reg_MsgPort;
	struct USB2_InterfaceHeader *	reg_Interface;
	struct RealFunctionNode *		reg_Function;
	struct USB2_Semaphore			reg_Semaphore;
	struct USB2_Header				reg_EPRHeader;
};

/***************************************************************************/

#endif // INC_PRIVATE_REGISTER_H
