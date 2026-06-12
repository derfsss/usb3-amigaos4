
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_HCD_UHCI_H
#define INC_PRIVATE_HCD_UHCI_H

/***************************************************************************/

/***************************************************************************/

struct _UHCI
{
	int dummy;
};

/***************************************************************************/

extern const struct HCDFunctions UHCIFunctions;

/***************************************************************************/

#endif // INC_PRIVATE_HCD_UHCI_H
