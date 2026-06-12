
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

SEC_CODE enum VSTAT __HCD_Valid( struct USBBase *usbbase UNUSED, struct USB2_HCDNode *hn, STR file UNUSED )

#else

SEC_CODE enum VSTAT __HCD_Valid( struct USBBase *usbbase UNUSED, struct USB2_HCDNode *hn )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__HCD_Valid" );

	/**/ if ( ! hn )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( hn == (PTR) 0x55555555 )
	{
		USBPANIC( "__HCD_Valid              : HN    %p : Free'd Memory : (%s)", hn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( hn->hn_StructID == 0 )
	{
		USBDEBUG( "__HCD_Valid              : HN    %p : Not Initalized : (%s)", hn, file);
		vstat = VSTAT_Null;
	} 
	else if ( hn->hn_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__HCD_Valid              : HN    %p : Structure allready freed : (%s)", hn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( hn->hn_StructID != ID_USB2_HN )
	{
		USBDEBUG( "__HCD_Valid              : HN    %p : Invalid ID $%08lx : (%s)", hn, hn->hn_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__HCD_Valid              : HN    %p : Stat %ld", hn, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
