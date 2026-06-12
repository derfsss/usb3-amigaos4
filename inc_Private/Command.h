
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_COMMAND_H
#define INC_PRIVATE_COMMAND_H

/***************************************************************************/

enum USB3_Cmd
{
	CMD_Unset = 0,

	// -- Start new Task/Process
	CMD_START_PROCESS,
	CMD_STOP_PROCESS,

	// -- Function Message
	CMD_FKT_DETACH,			// Send Detach msg too all Registers
};

// --

struct CmdHeader
{
	struct Message		cm_Message;
	enum USB3_Cmd		cm_Command;
	struct USBBase *	cm_usbbase;
	U32					cm_Result;
	struct Task *		cm_Task;
	U32					cm_Mask;
};

// --

struct FktMessage
{
	struct CmdHeader			fm_Cmd;
	struct RealFunctionNode *	fm_FN;
};

// --

/***************************************************************************/

#endif // INC_PRIVATE_COMMAND_H
