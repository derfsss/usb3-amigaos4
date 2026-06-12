
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

SEC_CODE static void myFree_DNs( struct USBBase *usbbase )
{
struct USB2_DriverNode *next;
struct USB2_DriverNode *dn;

//	USBDEBUG( "myFree_DNs :: Enter" );

	SEMAPHORE_OBTAIN( & usbbase->usb_Bind_Semaphore );

	dn = usbbase->usb_Bind_Header.uh_Head;

	while( dn )
	{
		next = Node_Next( dn );

		if (( dn->dn_Locks <= 0 ) && (( dn->dn_Detach ) || ( dn->dn_FreeMe )))
		{
			NODE_REMNODE( & usbbase->usb_Bind_Header, dn );

			if ( DRIVER_FREE( dn ) != FSTAT_Okay )
			{
				USBDEBUG( "myFree_DNs :: Unable to Free %p", dn );

				NODE_ADDHEAD( & usbbase->usb_Bind_Header, dn );
			}
			else
			{
//				usbbase->usb_IExec->DebugPrintF( "\n##\n## Driver Node %p freed\n##\n\n", dn );
			}
		}

		dn = next;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Bind_Semaphore );

//	USBDEBUG( "myFree_DNs :: Leave" );
}

// --
