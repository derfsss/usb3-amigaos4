
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Misc_Wait( struct USBBase *usbbase, U32 micro )
{
struct USB2_MsgPort mp;
struct TimeRequest req;

	USBDEBUG( "Wait( %ld.%02ld secs )", micro / 1000000, ( micro % 1000000 ) / 10000 );

// should not be needed
//	MEM_SET( & mp, 0, sizeof( mp ));

	if ( MSGPORT_INIT( & mp ))
	{
		MEM_COPY( usbbase->usb_TimeRequest, & req, sizeof( req ));

		req.Request.io_Message.mn_ReplyPort = & mp.ump_MsgPort;
		req.Request.io_Command = TR_ADDREQUEST;
		req.Time.Microseconds = micro % 1000000;
		req.Time.Seconds = micro / 1000000;

		IO_DO( & req );
	}
	else
	{
		USBDEBUG( "Failed to init MsgPort" );
	}

	MSGPORT_FREE( & mp );
}

// --
