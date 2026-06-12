
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE struct TagItem *__Misc_NextTagItem( struct USBBase *usbbase, struct TagItem **tag )
{
struct UtilityIFace *IUtility;
struct TagItem *tagitem;

	IUtility = usbbase->usb_IUtility;
	tagitem = IUtility->NextTagItem( tag );

	return( tagitem );
}

// --
