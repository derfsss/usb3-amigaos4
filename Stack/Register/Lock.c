
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum LSTAT __Register_Lock( struct USBBase *usbbase, struct RealRegister *reg, STR file UNUSED )

#else

SEC_CODE enum LSTAT __Register_Lock( struct USBBase *usbbase, struct RealRegister *reg )

#endif

{
enum LSTAT lstat;

	TASK_NAME_ENTER( "__Register_Lock" );

	USBDEBUG( "__Register_Lock          : REG   %p : Locks %2ld (old) : (%s)", reg, ( reg )?reg->reg_Locks:0, (file)?file:"<NULL>" );

	usbbase->usb_IExec->DebugPrintF( "USB: RegLock: sem obtain [%s] reg=%p\n",
		usbbase->usb_IExec->FindTask(NULL)->tc_Node.ln_Name, reg );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	usbbase->usb_IExec->DebugPrintF( "USB: RegLock: sem GOT [%s]\n",
		usbbase->usb_IExec->FindTask(NULL)->tc_Node.ln_Name );

	if ( REGISTER_VALID( reg ) == VSTAT_Okay )
	{
		reg->reg_Locks++;
		lstat = LSTAT_Okay;

//		USBDEBUG( "__Register_Lock          : REG   %p : New Lock Count : %ld (+1)", reg, reg->reg_Locks );
	}
	else
	{
		USBPANIC( "__Register_Lock          : REG   %p : Lock failed : (%s)", reg, file );
		lstat = LSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	TASK_NAME_LEAVE();

	return( lstat );
}

// --
