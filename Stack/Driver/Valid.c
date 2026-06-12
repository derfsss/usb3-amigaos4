
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

SEC_CODE enum VSTAT __Driver_Valid( struct USBBase *usbbase UNUSED, struct USB2_DriverNode *dn, STR file UNUSED )

#else

SEC_CODE enum VSTAT __Driver_Valid( struct USBBase *usbbase UNUSED, struct USB2_DriverNode *dn )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Driver_Valid" );

	/**/ if ( ! dn )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( dn == (PTR) 0x55555555 )
	{
		USBPANIC( "__Driver_Valid           : DN    %p : Free'd Memory : (%s)", dn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( dn->dn_StructID == 0 )
	{
		USBDEBUG( "__Driver_Valid           : DN    %p : Not Initalized : (%s)", dn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( dn->dn_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__Driver_Valid           : DN    %p : Structure allready freed : (%s)", dn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( dn->dn_StructID != ID_USB2_DN )
	{
		USBDEBUG( "__Driver_Valid           : DN    %p : Invalid ID $%08lx : (%s)", dn, dn->dn_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Driver_Valid           : DN    %p : Stat %ld", dn, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
