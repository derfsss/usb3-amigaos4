
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_USB3_SIGNAL_H
#define INC_PRIVATE_USB3_SIGNAL_H

/***************************************************************************/

struct USB3_Signal
{
	U32								sig_StructID;
	S32								sig_Signal_Nr;
	U32								sig_Signal_Mask;
};

/***************************************************************************/

#endif // INC_PRIVATE_USB3_SIGNAL_H
