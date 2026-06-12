
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Cmd_Fkt_Detach( struct USBBase *usbbase, struct intern *in UNUSED, struct FktMessage *tm )
{
struct RealFunctionNode *fn;
struct USB2_DriverNode *dn;
S32 reply;

	USBERROR( "__Cmd_Fkt_Detach" );

	reply = FALSE;

	// --

	fn = tm->fm_FN;

	if ( FUNCTION_VALID( fn ) == VSTAT_Okay )
	{

//		USBERROR( "Obtain 15" );
//		SEMAPHORE_OBTAIN( & usbbase->usb_Bind_Semaphore );

		dn = usbbase->usb_Bind_Header.uh_Head;

		while( dn )
		{
			USBERROR( "__Cmd_Fkt_Detach : DN    %p", dn );

			if ( dn->dn_Function == fn )
			{
				USBERROR( "__Cmd_Fkt_Detach : Sending msg to %p", dn );

				// todo send a message too Driver/Register about Detach
			}

			dn = Node_Next( dn );
		}

//		SEMAPHORE_RELEASE( & usbbase->usb_Bind_Semaphore );

		FUNCTION_UNLOCK( fn );
	}

	MEM_FREEVEC( tm );

	// --

	return( reply );
}

// --
