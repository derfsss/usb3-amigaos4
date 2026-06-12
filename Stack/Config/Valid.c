
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

SEC_CODE enum VSTAT __Config_Valid( struct USBBase *usbbase UNUSED, struct USB3_ConfigNode *cn, STR file UNUSED )

#else

SEC_CODE enum VSTAT __Config_Valid( struct USBBase *usbbase UNUSED, struct USB3_ConfigNode *cn )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Config_Valid" );

	/**/ if ( ! cn )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( cn == (PTR) 0x55555555 )
	{
		USBPANIC( "__Config_Valid           : CN    %p : Free'd Memory : (%s)", cn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( cn->cfg_StructID == 0 )
	{
		USBDEBUG( "__Config_Valid           : CN    %p : Not Initalized : (%s)", cn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( cn->cfg_StructID == ID_USB3_FREED )
	{
		USBDEBUG( "__Config_Valid           : CN    %p : Structure allready freed : (%s)", cn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( cn->cfg_StructID != ID_USB3_CFG )
	{
		USBDEBUG( "__Config_Valid           : CN    %p : Invalid ID $%08lx : (%s)", cn, cn->cfg_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Config_Valid           : CN    %p : Stat %ld", cn, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
