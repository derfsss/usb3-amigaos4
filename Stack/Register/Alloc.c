
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

SEC_CODE struct RealRegister *__Register_Alloc( struct USBBase *usbbase, STR file UNUSED )

#else

SEC_CODE struct RealRegister *__Register_Alloc( struct USBBase *usbbase )

#endif

{
struct RealRegister *reg;
S32 error;

	error = TRUE;

	USBDEBUG( "__Register_Alloc         : (%s)", (file)?file:"<NULL>" );

	reg = MEMORY_ALLOC( MEMID_USBRegister, TRUE );

	if ( ! reg )
	{
		USBERROR( "__Register_Alloc         : Error allocating memory" );
		goto bailout;
	}

	reg->reg_StructID = ID_USB3_REG;

	// --

	SEMAPHORE_INIT( & reg->reg_Semaphore );

	if ( ! MSGPORT_INIT( & reg->reg_MsgPort ))
	{
		USBERROR( "__Register_Alloc         : Error initing MsgPort" );
		goto bailout;
	}

//	reg->reg_Public.Stack_MsgPort		= & reg->reg_MsgPort.ump_MsgPort;
//	reg->reg_Public.Stack_MsgPortBit	= reg->reg_MsgPort.ump_Signal.sig_Signal_Mask;

	// --

	error = FALSE;

bailout:

	if (( reg ) && ( error ))
	{
		REGISTER_FREE( reg );
		reg = NULL;
	}

	USBDEBUG( "__Register_Alloc         : REG   %p : (new)", reg );

	return( reg );
}

// --
