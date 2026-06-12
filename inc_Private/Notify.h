
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_NOTIFY_H
#define INC_PRIVATE_NOTIFY_H

/***************************************************************************/

struct USB2_NotifyNode
{
	struct USB2_Node				nn_Node;

	// --
	U32								nn_StructID;
	struct MsgPort *				nn_MsgPort;			// Normal Exec Port from User
	U32								nn_Type;
};

/***************************************************************************/

#endif // INC_PRIVATE_NOTIFY_H
