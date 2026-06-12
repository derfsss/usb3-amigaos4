
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_USB3_MSGPORT_H
#define INC_PRIVATE_USB3_MSGPORT_H

/***************************************************************************/

struct USB3_MsgPort
{
	U32								ump_StructID;
	struct MsgPort					ump_MsgPort;
	struct USB3_Signal				ump_Signal;
};

/***************************************************************************/

#endif // INC_PRIVATE_USB3_MSGPORT_H
