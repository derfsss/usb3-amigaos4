
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

SEC_CODE static void __myTask_Free( struct USBBase *usbbase, struct intern *in )
{
	USBINFO( "__myTask_Free (Master)" );

	__myTask_Def_Free( usbbase, in );
}

// --
