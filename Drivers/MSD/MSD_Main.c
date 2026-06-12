
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --
#if 1

SEC_CODE void MSD_Main( struct USBBase *usbbase, struct MSDDevice *msddev )
{
	TASK_NAME_ENTER( "MSD : MSD_Main" );
	USBERROR( "MSD : MSD_Main : Enter" );

	// --
	// Normal running loop
	MSD_Main_1_Normal( usbbase, msddev );

	// --
	// The device is detaching (unplugged or stack shutdown): denounce
	// every LUN and fire the disk-change interrupts NOW, so volumes
	// disappear immediately instead of on the next failed I/O
	MSD_Device_Detach( usbbase, msddev );

	// --
	// Stopping loop, wait for Resources to be released
	MSD_Main_2_Stopping( usbbase, msddev );

	// --
	// Start shutdown
//	__Shutdown( usbbase, in );

	// --
	// -- Exit

	USBERROR( "MSD : MSD_Main : Leave" );
	TASK_NAME_LEAVE();
}

#endif
// --
