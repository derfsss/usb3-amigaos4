
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Semaphore_Free( struct USBBase *usbbase UNUSED, struct USB2_Semaphore *us )
{
	if (( us == NULL )
	||	( us->us_StructID == 0 ) 
	||	( us->us_StructID == ID_USB2_FREED ))
	{
		return;
	}

	#ifdef DO_PANIC

	if ( us->us_StructID != ID_USB2_SEMA )
	{
		USBPANIC( "Semaphore_Free  :  1 : Invalid ID" );
	}

	#endif

	us->us_StructID = ID_USB2_FREED;
}

// --
