
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef STACK_HCD_HCD_H
#define STACK_HCD_HCD_H

/***************************************************************************/

struct intern
{
	#ifdef DO_DEBUG
	U32						StructID;
	#endif

	// -- Timer Tick
	struct USB2_MsgPort		Tick_MsgPort;
	S32						Tick_TimerAdded;
	struct TimeRequest		Tick_TimeRequest;
};

/***************************************************************************/

#endif // STACK_HCD_HCD_H
