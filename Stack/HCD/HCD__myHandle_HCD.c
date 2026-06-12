
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __myHandle_HCD( struct USBBase *usbbase UNUSED, struct USB3_HCDNode *hn, U32 mask )
{
	if ( hn->HCD_Functions.Handler_HCD )
	{
		 hn->HCD_Functions.Handler_HCD( hn, mask );
	}
	#ifdef DO_DEBUG
	else
	{
		USBPANIC( "__myMain (HCD) : __Normal : HCD_Mask : Implement me" );
	}
	#endif
}

// --
