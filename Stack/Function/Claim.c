
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Function_Claim( struct USBBase *usbbase, struct RealRegister *reg, struct RealFunctionNode *fn )
{
S32 retval;

	USBDEBUG( "Trying to claim Function from Reg: %p, FN: %p", reg, reg->reg_Function );

	retval = FALSE;

	// Check input and make sure it stays put
	if ( REGISTER_LOCK( reg ) == LSTAT_Okay )
	{
		// Check input and make sure it stays put
		if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
		{
			SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

			while( TRUE )
			{
				if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
				{
					USBDEBUG( "__Function_Claim : Function Detached" );
					break;
				}

				if ( fn->fkt_Owner )
				{
					USBDEBUG( "__Function_Claim : Function allready claimed" );
					break;
				}

				// You are allowed to Claim Function and then Interfaces (same owner)
				// but you are NOT allowed to claim Interfaces and then Function
				if ( fn->fkt_InterfaceClaimLocks )
				{
					USBDEBUG( "__Function_Claim : Function allready claimed" );
					break;
				}

				// hmm should I not Lock Reg instead?
				if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
				{
					USBDEBUG( "__Function_Claim : Function lock failed" );
					break;
				}

				fn->fkt_Owner = reg;
				retval = TRUE;
				break;
			}

			SEMAPHORE_RELEASE( & fn->fkt_Semaphore );
			FUNCTION_UNLOCK( fn );
		}
		else
		{
			USBDEBUG( "__Function_Claim : Invalid FN Node" );
		}

		REGISTER_UNLOCK( reg );
	}
	else
	{
		USBDEBUG( "__Function_Claim : Invalid Reg Node" );
	}

	return( retval );
}

// --
