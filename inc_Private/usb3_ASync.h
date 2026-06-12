
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_USB3_ASYNC_H
#define INC_PRIVATE_USB3_ASYNC_H

/***************************************************************************/

struct USB3_ASync
{
	U32								ua_StructID;
	struct USB3_Semaphore			ua_Semaphore;
	struct Task *					ua_Parent;			// used for ASync_Sub .. gets CTRL+D
	struct USB3_Signal				ua_Signal;			// used for ASync_Wait
	struct Task *					ua_Task;			// used for ASync_Wait .. gets ua_Signal
	S32								ua_Counter;
};

/***************************************************************************/

#endif // INC_PRIVATE_USB3_ASYNC_H
