
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

SEC_CODE void __Register_Unlock( struct USBBase *usbbase, struct RealRegister *reg, STR file UNUSED )

#else

SEC_CODE void __Register_Unlock( struct USBBase *usbbase, struct RealRegister *reg )

#endif

{
	TASK_NAME_ENTER( "__Register_Unlock" );

	USBDEBUG( "__Register_Unlock        : REG   %p : Locks %2ld (old) : (%s)", reg, ( reg )?reg->reg_Locks:0, (file)?file:"<NULL>" );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	if ( REGISTER_VALID( reg ) == VSTAT_Okay )
	{
		if ( reg->reg_Locks > 0 )
		{
			reg->reg_Locks--;

//			USBDEBUG( "__Register_Unlock        : REG   %p : New Lock Count : %ld (-1)", reg, reg->reg_Locks );
		}
		else
		{
			USBPANIC( "__Register_Unlock        : REG   %p : Register Node NOT Locked (%ld)", reg, reg->reg_Locks );
		}
	}
	else
	{
		USBERROR( "__Register_Unlock        : REG   %p : Invalid Register Node", reg );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();
}

// --
