
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

S32 Unit_Startup_USB( struct EthernetUnit *unit )
{
S32 retval;
S32 stat;

	USBDEBUG( "RTL8188 : Unit_Startup_USB : Enter" );

	retval = FALSE;

	// --
	// Small speedup.. only check if IUSB3 is null

	if ( ! IUSB3 )
	{
		// --
		// Check if usb3 is running

		if ( ! FindName( & ((struct ExecBase *)SysBase)->DeviceList, "usb3.device" ))
		{
			USBERROR( "RTL8188 : Error USB3 stack not running" );

			if ( ! unit->unit_NotRunning )
			{
				// Only print one pr Opener
				unit->unit_NotRunning = TRUE;	

				TimedDosRequesterTags( 
					TDR_TitleString, DEVNAME,
					TDR_GadgetString, "_Okay",
					TDR_ImageType, TDRIMAGE_ERROR,
					TDR_FormatString, "Error USB3 stack\nnot running",
					TAG_END
				);
			}
			goto bailout;
		}

		if ( ! USB3MsgPort )
		{
			// We do not alloc a signal,
			// IOReq much NOT be used data transfer
			USB3MsgPort = AllocSysObjectTags( ASOT_PORT,
				ASOPORT_AllocSig, FALSE,
				TAG_END
			);

			if ( ! USB3MsgPort )
			{
				USBERROR( "RTL8188 : Error creating USB3 MsgPort" );
				goto bailout;
			}

			USBERROR( "MsgPort sig %ld", USB3MsgPort->mp_SigBit );
		}
		// --
		// Check IOReq

		if ( ! USB3IOReq )
		{
			USB3IOReq = AllocSysObjectTags( ASOT_IOREQUEST,
				ASOIOR_Size, sizeof( struct IORequest ),
				ASOIOR_ReplyPort, USB3MsgPort,
				TAG_END
			);

			if ( ! USB3IOReq )
			{
				USBERROR( "RTL8188 : Error creating USB3 IORequest" );
				goto bailout;
			}
		}

		// --
		// Check Open?

		if ( ! USB3Base )
		{
			stat = OpenDevice( "usb3.device", 0, USB3IOReq, 0 );

			if ( stat )
			{
				USBERROR( "RTL8188 : Error opening USB3 Device" );
				goto bailout;
			}

			USB3Base = (PTR) USB3IOReq->io_Device;
		}

		// --
		// Check Interface

//		if ( ! IUSB3 )
		{
			IUSB3 = (PTR) GetInterface( (PTR) USB3Base, "main", 1, NULL );

			if ( ! IUSB3 )
			{
				USBERROR( "RTL8188 : Error getting USB3 Interface" );
				goto bailout;
			}
		}
	}

	// --

	retval = TRUE;

bailout:

	USBDEBUG( "RTL8188 : Unit_Startup_USB : Leave : Retval %ld", retval );

	return( retval );
}