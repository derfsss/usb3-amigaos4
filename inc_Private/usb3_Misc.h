
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_USB3_MISC_H
#define INC_PRIVATE_USB3_MISC_H

/***************************************************************************/

struct RealSetupData
{
	struct USB3_SetupData			rsd_Public;					// At the moment must be first, todo: do same as EPResource or ?
	U32								rsd_StructID;
	U32								rsd_Phy;
};

/***************************************************************************/

#endif // INC_PRIVATE_USB3_MISC_H
