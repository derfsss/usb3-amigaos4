
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

SEC_CODE enum VSTAT __Function_Valid( struct USBBase *usbbase UNUSED, struct RealFunctionNode *fn, STR file UNUSED )

#else

SEC_CODE enum VSTAT __Function_Valid( struct USBBase *usbbase UNUSED, struct RealFunctionNode *fn )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Function_Valid" );

	/**/ if ( ! fn )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( fn == (PTR) 0x55555555 )
	{
		USBPANIC( "__Function_Valid         : FN    %p : Free'd Memory : (%s)", fn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( fn->fkt_StructID == 0 )
	{
//		usbbase->usb_IExec->DebugPrintF( "__Function_Valid         : FN    %p : Not Initalized : (%s)\n", fn, (file)?file:"<NULL>" );
		USBDEBUG( "__Function_Valid         : FN    %p : Not Initalized : (%s)", fn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( fn->fkt_StructID == ID_USB2_FREED )
	{
//		usbbase->usb_IExec->DebugPrintF( "__Function_Valid         : FN    %p : Structure allready freed : (%s)\n", fn, (file)?file:"<NULL>" );
		USBDEBUG( "__Function_Valid         : FN    %p : Structure allready freed : (%s)", fn, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( fn->fkt_StructID != ID_USB2_FKT )
	{
//		usbbase->usb_IExec->DebugPrintF( "__Function_Valid         : FN    %p : Invalid ID $%08lx : (%s)\n", fn, fn->fkt_StructID, (file)?file:"<NULL>" );
		USBDEBUG( "__Function_Valid         : FN    %p : Invalid ID $%08lx : (%s)", fn, fn->fkt_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Function_Valid         : FN    %p : Stat %ld", fn, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
