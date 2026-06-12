
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Misc_IceColdReboot( struct USBBase *usbbase )
{
	USBDEBUG( "IceCold Reboot" );

	usbbase->usb_IExec->IceColdReboot();
}

// --
