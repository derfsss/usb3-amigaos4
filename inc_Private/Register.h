
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
	struct USB3_Register			reg_Public;					// At the moment must be first, todo: do same as EPResource or ?

	// -- 
	U32								reg_StructID;
	S32								reg_Locks;
	U16								reg_Detach;
	U16								reg_FreeMe;
//	struct USB3_TaskNode *			reg_Task; 
	struct USB3_ASync *				reg_ASync;
	// --

	STR								reg_Title;
//	struct USB3_EPResource *	reg_Control;
	struct USB3_MsgPort				reg_MsgPort;
	struct USB3_InterfaceHeader *	reg_Interface;
	struct RealFunctionNode *		reg_Function;
	struct USB3_Semaphore			reg_Semaphore;
	struct USB3_Header				reg_EPRHeader;
};

/***************************************************************************/

#endif // INC_PRIVATE_REGISTER_H
