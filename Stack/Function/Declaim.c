
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Function_Declaim( struct USBBase *usbbase, struct RealRegister *reg, struct RealFunctionNode *fn )
{
	USBDEBUG( "Trying to declaim Function from Reg: %p, FN: %p", reg, reg->reg_Function );

	// Check input and make sure it stays put
	if ( REGISTER_LOCK( reg ) == LSTAT_Okay )
	{
		// Check input and make sure it stays put
		if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
		{
			SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

			while( TRUE )
			{
				if ( ! fn->fkt_Owner )
				{
					USBDEBUG( "__Function_Declaim  : Function not claimed" );
					break;
				}

				if ( fn->fkt_Owner != reg )
				{
					USBDEBUG( "__Function_Declaim  : Wrong Owner" );
					break;
				}

				fn->fkt_Owner = NULL;
				// hmm should I not Lock Reg instead?
				FUNCTION_UNLOCK( fn );
				break;
			}
		
			SEMAPHORE_RELEASE( & fn->fkt_Semaphore );
			FUNCTION_UNLOCK( fn );
		}
		else
		{
			USBDEBUG( "__Function_Declaim : Invalid fn Node" );
		}

		REGISTER_UNLOCK( reg );
	}
	else
	{
		USBDEBUG( "__Function_Declaim : Invalid reg Node" );
	}
}

// --
