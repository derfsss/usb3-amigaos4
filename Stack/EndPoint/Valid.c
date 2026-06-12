
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

SEC_CODE enum VSTAT __EndPoint_Valid( struct USBBase *usbbase UNUSED, struct USB3_EndPointNode *ep, STR file UNUSED )

#else

SEC_CODE enum VSTAT __EndPoint_Valid( struct USBBase *usbbase UNUSED, struct USB3_EndPointNode *ep )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__EndPoint_Valid" );

	/**/ if ( ! ep )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( ep == (PTR) 0x55555555 )
	{
		USBPANIC( "__EndPoint_Valid         : EP    %p : Free'd Memory : (%s)", ep, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( ep->ep_StructID == 0 )
	{
		USBDEBUG( "__EndPoint_Valid         : EP    %p : Not Initalized : (%s)", ep, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( ep->ep_StructID == ID_USB3_FREED )
	{
		USBDEBUG( "__EndPoint_Valid         : EP    %p : Structure allready freed : (%s)", ep, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( ep->ep_StructID != ID_USB3_EP )
	{
		USBDEBUG( "__EndPoint_Valid         : EP    %p : Invalid ID $%08lx : (%s)", ep, ep->ep_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__EndPoint_Valid         : EP    %p : Stat %ld", ep, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
