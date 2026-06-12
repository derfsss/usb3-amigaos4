
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

SEC_CODE enum VSTAT __Interface_ValidHeader( struct USBBase *usbbase UNUSED, struct USB2_InterfaceHeader *ih, STR file )

#else

SEC_CODE enum VSTAT __Interface_ValidHeader( struct USBBase *usbbase UNUSED, struct USB2_InterfaceHeader *ih )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Interface_ValidHeader" );

	/**/ if ( ! ih )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( ih == (PTR) 0x55555555 )
	{
		USBPANIC( "__Interface_ValidHeader  : IH    %p : Free'd Memory : (%s)", ih, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( ih->ih_StructID == 0 )
	{
		USBDEBUG( "__Interface_ValidHeader  : IH    %p : Not Initalized", ih );
		vstat = VSTAT_Null;
	} 
	else if ( ih->ih_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__Interface_ValidHeader  : IH    %p : Free Struct", ih );
		vstat = VSTAT_Null;
	}
	else if ( ih->ih_StructID != ID_USB2_IFCH )
	{
		USBDEBUG( "__Interface_ValidHeader  : IH    %p : Invalid ID ($%08lx)", ih, ih->ih_StructID );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Interface_ValidHeader  : IH    %p : Stat %ld", ih, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
