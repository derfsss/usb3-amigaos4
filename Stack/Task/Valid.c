
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

SEC_CODE enum VSTAT __Task_Valid( struct USBBase *usbbase UNUSED, struct USB2_TaskNode *tn, STR file UNUSED )

#else

SEC_CODE enum VSTAT __Task_Valid( struct USBBase *usbbase UNUSED, struct USB2_TaskNode *tn )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Task_Valid" );

	/**/ if ( ! tn )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( tn == (PTR) 0x55555555 )
	{
		USBPANIC( "__Task_Valid             : TN %p : Free'd Memory : (%s)", tn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( tn->tn_StructID == 0 )
	{
		USBDEBUG( "__Task_Valid             : TN %p : Not Initalized : (%s)", tn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( tn->tn_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__Task_Valid             : TN %p : Structure allready freed : (%s)", tn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( tn->tn_StructID != ID_USB2_TN )
	{
		USBDEBUG( "__Task_Valid             : TN %p : Invalid ID $%08lx : (%s)", tn, tn->tn_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Task_Valid             : TN %p : Stat %ld", tn, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
