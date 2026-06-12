
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

SEC_CODE enum VSTAT __FDriver_Valid( struct USBBase *usbbase UNUSED, struct USB3_FktDriverNode *fdn, STR file UNUSED )

#else

SEC_CODE enum VSTAT __FDriver_Valid( struct USBBase *usbbase UNUSED, struct USB3_FktDriverNode *fdn )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__FDriver_Valid" );

	/**/ if ( ! fdn )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( fdn == (PTR) 0x55555555 )
	{
		USBPANIC( "__FDriver_Valid          : FDN %p : Free'd Memory : (%s)", fdn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( fdn->fdn_StructID == 0 )
	{
		USBDEBUG( "__FDriver_Valid          : FDN %p : Not Initalized : (%s)", fdn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( fdn->fdn_StructID == ID_USB3_FREED )
	{
		USBDEBUG( "__FDriver_Valid          : FDN %p : Structure allready freed : (%s)", fdn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( fdn->fdn_StructID != ID_USB3_FDN )
	{
		USBDEBUG( "__FDriver_Valid          : FDN %p : Invalid ID $%08lx : (%s)", fdn, fdn->fdn_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__FDriver_Valid          : FDN %p : Stat %ld", fdn, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
