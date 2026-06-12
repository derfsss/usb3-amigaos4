
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Interface_ClaimHeader( struct USBBase *usbbase, struct RealRegister *reg, struct USB3_InterfaceHeader *ih )
{
struct RealFunctionNode *fn;
S32 retval;

	USBDEBUG( "Trying to claim Interface from Reg: %p, IH: %p", reg, reg->reg_Interface );

	retval = FALSE;

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
				if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
				{
					USBERROR( "__Interface_ClaimHeader : Function Detached : FN    %p", fn );
					break;
				}

				// You are allowed to Claim Function and then Interfaces (same owner)
				// but you are NOT allowed to claim Interfaces and then Function
				if (( fn->fkt_Owner ) && ( fn->fkt_Owner != reg ))
				{
					USBERROR( "__Interface_ClaimHeader : Wrong Function Owner" );
					break;
				}

				// Should I allowed Owner claiming it self again??
				if ( ih->ih_Owner )
				{
					USBERROR( "__Interface_ClaimHeader : Interface allready claimed" );
					break;
				}

				// hmm should I not Lock Reg instead?
				if ( INTERFACE_LOCKHEADER(ih) != LSTAT_Okay )
				{
					USBERROR( "__Interface_ClaimHeader : Interface lock failed : IH    %p", ih );
					break;
				}

				fn->fkt_InterfaceClaimLocks++;
				ih->ih_Owner = reg;
				retval = TRUE;
				break;
			}

			SEMAPHORE_RELEASE( & fn->fkt_Semaphore );
			INTERFACE_UNLOCKHEADER(ih);
		}
		else
		{
			USBERROR( "__Interface_ClaimHeader : Invalid IH Node : IH    %p", ih );
		}

		REGISTER_UNLOCK( reg );
	}
	else
	{
		USBERROR( "__Interface_ClaimHeader : Invalid Reg Node : REG   %p", reg );
	}

	return( retval );
}

// --
