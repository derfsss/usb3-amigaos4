
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

SEC_CODE enum VSTAT __Register_Valid( struct USBBase *usbbase UNUSED, struct RealRegister *reg, STR file UNUSED )

#else

SEC_CODE enum VSTAT __Register_Valid( struct USBBase *usbbase UNUSED, struct RealRegister *reg )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Register_Valid" );

	/**/ if ( ! reg )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( reg == (PTR) 0x55555555 )
	{
		USBPANIC( "__Register_Valid         : REG   %p : Free'd Memory : (%s)", reg, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( reg->reg_StructID == 0 )
	{
		USBDEBUG( "__Register_Valid         : REG   %p : Not Initalized : (%s)", reg, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	} 
	else if ( reg->reg_StructID == ID_USB2_FREED )
	{
		USBDEBUG( "__Register_Valid         : REG   %p : Structure allready freed : (%s)", reg, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}
	else if ( reg->reg_StructID != ID_USB2_REG )
	{
		USBDEBUG( "__Register_Valid         : REG   %p : Invalid ID $%08lx : (%s)", reg, reg->reg_StructID, (file)?file:"<NULL>" );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Register_Valid         : REG   %p : Stat %ld", reg, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
