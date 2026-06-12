
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Interface_DeclaimHeader( struct USBBase *usbbase, struct RealRegister *reg, struct USB2_InterfaceHeader *ih )
{
struct RealFunctionNode *fn;

	USBDEBUG( "Trying to declaim Interface from Reg: %p, IH: %p", reg, reg->reg_Interface );

	// Check input and make sure it stays put
	if ( REGISTER_LOCK( reg ) == LSTAT_Okay )
	{
		// Check input and make sure it stays put
		if ( INTERFACE_LOCKHEADER(ih) == LSTAT_Okay )
		{
			fn = ih->ih_Function;
			SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

			while( TRUE )
			{
				if ( ! ih->ih_Owner )
				{
					USBDEBUG( "__Interface_DeclaimHeader  : Interface not claimed" );
					break;
				}

				if ( ih->ih_Owner != reg )
				{
					USBDEBUG( "__Interface_DeclaimHeader  : Wrong Owner" );
					break;
				}

				fn->fkt_InterfaceClaimLocks--;
				ih->ih_Owner = NULL;
				// hmm should I not Lock Reg instead?
				INTERFACE_UNLOCKHEADER(ih);
				break;
			}
		
			SEMAPHORE_RELEASE( & fn->fkt_Semaphore );
			INTERFACE_UNLOCKHEADER(ih);
		}
		else
		{
			USBDEBUG( "__Function_Declaim : Invalid ih Node" );
		}

		REGISTER_UNLOCK( reg );
	}
	else
	{
		USBDEBUG( "__Function_Declaim : Invalid reg Node" );
	}
}

// --
