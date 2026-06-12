
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

SEC_CODE enum VSTAT __TaskMsg_Valid( struct USBBase *usbbase UNUSED, struct USB3_TaskMsg *tm, STR file UNUSED )

#else

SEC_CODE enum VSTAT __TaskMsg_Valid( struct USBBase *usbbase UNUSED, struct USB3_TaskMsg *tm )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__TaskMsg_Valid" );

	/**/ if ( ! tm )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( tm == (PTR) 0x55555555 )
	{
		USBPANIC( "__TaskMsg_Valid          : TM %p : Free'd Memory : (%s)", tm, (file)?file:"<NULL>" );		
		vstat = VSTAT_Null;
	}

	#endif

	else if ( tm->tm_StructID == 0 )
	{
		USBDEBUG( "__TaskMsg_Valid          : TM %p : Not Initalized : (%s)", tm, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( tm->tm_StructID == ID_USB3_FREED )
	{
		USBDEBUG( "__TaskMsg_Valid          : TM %p : Structure allready freed : (%s)", tm, (file)?file:"<NULL>" );		
		vstat = VSTAT_Null;
	}
	else if ( tm->tm_StructID != ID_USB3_TMSG )
	{
		USBDEBUG( "__TaskMsg_Valid          : TM %p : Invalid ID $%08lx : (%s)", tm, tm->tm_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__TaskMsg_Valid          : TM %p : Stat %ld", tm, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
