
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

SEC_CODE enum VSTAT __EndPointRes_Valid( struct USBBase *usbbase UNUSED, struct RealEndPointResource *epr, STR file UNUSED )

#else

SEC_CODE enum VSTAT __EndPointRes_Valid( struct USBBase *usbbase UNUSED, struct RealEndPointResource *epr )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__EndPointRes_Valid" );

	/**/ if ( ! epr )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( epr == (PTR) 0x55555555 )
	{
		USBPANIC( "__EndPointRes_Valid      : EPR   %p : Free'd Memory : (%s)", epr, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( epr->epr_StructID == 0 )
	{
		USBDEBUG( "__EndPointRes_Valid      : EPR   %p : Not Initalized : (%s)", epr, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( epr->epr_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__EndPointRes_Valid      : EPR   %p : Structure allready freed : (%s)", epr, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( epr->epr_StructID != ID_USB2_EPR )
	{
		USBDEBUG( "__EndPointRes_Valid      : EPR   %p : Invalid ID $%08lx : (%s)", epr, epr->epr_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__EndPointRes_Valid      : EPR   %p : Stat %ld", epr, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
