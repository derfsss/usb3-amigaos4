
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum VSTAT __IORequest_Valid( struct USBBase *usbbase UNUSED, struct RealRequest *ioreq, STR file UNUSED )

#else

SEC_CODE enum VSTAT __IORequest_Valid( struct USBBase *usbbase UNUSED, struct RealRequest *ioreq )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__IORequest_Valid" );

	/**/ if ( ! ioreq )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( ioreq == (PTR) 0x55555555 )
	{
		USBPANIC( "__IORequest_Valid        : IOReq %p : Free'd Memory : (%s)", ioreq, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( ioreq->req_StructID == 0 )
	{
		USBDEBUG( "__IORequest_Valid        : IOReq %p : Not Initalized : (%s)", ioreq, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( ioreq->req_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__IORequest_Valid        : IOReq %p : Structure allready freed : (%s)", ioreq, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( ioreq->req_StructID != ID_USB2_IOR )
	{
		// We also use this for testing on TimeRequest
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
