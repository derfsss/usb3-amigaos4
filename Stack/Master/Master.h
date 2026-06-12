
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef _STACK_MASTER_H_
#define _STACK_MASTER_H_

#define MAIN_RETURN_UNSET		0
#define MAIN_RETURN_EXIT		1
#define MAIN_RETURN_PROMOTE		2

struct intern
{
	#ifdef DO_DEBUG
	U32						StructID;
	#endif

	S32						Stopping;

	// -- Command MsgPort for Extern Commands
	struct USB3_MsgPort		Cmd_MsgPort;
	// -- Notify Replys
	struct USB3_MsgPort		Notify_ReplyMsgPort;

	// -- (Sub) Tasks Shutdown -- TaskMsg's
	struct USB3_Header		Shutdown_TM_List;
	struct USB3_Signal		Task_Shutdown_Signal;
	struct USB3_MsgPort		Task_Shutdown_MsgPort;

	// -- Timer Tick
	struct USB3_MsgPort		Tick_MsgPort;
	S32						Tick_TimerAdded;
	struct TimeRequest		Tick_TimeRequest;
};

// --

struct MasterStartMsg
{
	PTR		usbbase;
	PTR		Parent;
	U32		Result;
	U32		Mask;
};

#endif

// --
