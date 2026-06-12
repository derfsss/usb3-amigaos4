
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __Free_3_Rem_HUB( struct USBBase *usbbase UNUSED, struct USB3_HCDNode *hn, struct intern *in UNUSED )
{
struct RealFunctionNode *fn;

	// -- Free RootHUB Entr

	if ( hn->hn_Function_Array )
	{
		// RootHUB may be around
		fn = hn->hn_Function_Array[0];

		if ( fn )
		{
			TASK_STOP_FN( fn );

			if ( FUNCTION_FREE(fn) == FSTAT_Okay )
			{
				hn->hn_Function_Array[0] = NULL;
			}
		}

		// --

		#ifdef DO_DEBUG

		for( U32 cnt=0 ; cnt<128 ; cnt++ )
		{
			if ( hn->hn_Function_Array[cnt] )
			{
				USBDEBUG( "__myFree (HCD)           : --> HCD Function #%ld : Error Function %p (Still Around)", cnt, hn->hn_Function_Array[cnt] );
			}
		}

		#endif
	}

	// --
}

// --
