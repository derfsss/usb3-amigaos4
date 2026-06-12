
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

SEC_CODE enum VSTAT __Interface_ValidGroup( struct USBBase *usbbase UNUSED, struct USB2_InterfaceGroup *ig, STR file )

#else

SEC_CODE enum VSTAT __Interface_ValidGroup( struct USBBase *usbbase UNUSED, struct USB2_InterfaceGroup *ig )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Interface_ValidGroup" );

	/**/ if ( ! ig )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( ig == (PTR) 0x55555555 )
	{
		USBPANIC( "__Interface_ValidGroup   : IG    %p : Free'd Memory : (%s)", ig, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( ig->ig_StructID == 0 )
	{
		USBDEBUG( "__Interface_ValidGroup   : IG    %p : Not Initalized", ig );
		vstat = VSTAT_Null;
	} 
	else if ( ig->ig_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__Interface_ValidGroup   : IG    %p : Free Struct", ig );
		vstat = VSTAT_Null;
	}
	else if ( ig->ig_StructID != ID_USB2_IFCG )
	{
		USBDEBUG( "__Interface_ValidGroup   : IG     %p : Invalid ID ($%08lx)", ig, ig->ig_StructID );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Interface_ValidGroup   : IG    %p : Stat %ld", ig, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
