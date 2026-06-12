
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE U32 __HCD_Addr_Obtain( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealFunctionNode *fn )
{
U32 retval;
U32 adr;

	TASK_NAME_ENTER( "HCD : __HCD_Addr_Obtain" );

	retval = 0;	

	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBDEBUG( "__HCD_Addr_Obtain : Invalid HN %p", hn );
		goto bailout;
	}

	if ( FUNCTION_VALID(fn) != VSTAT_Okay )
	{
		USBDEBUG( "__HCD_Addr_Obtain : Invalid FN %p", fn );
		goto bailout;
	}

	SEMAPHORE_OBTAIN( & usbbase->usb_Addr_Semaphore );

	// Find Free Address

	for( adr=1 ; adr<128 ; adr++ )
	{
		if ( ! hn->hn_Function_Array[adr] )
		{
			USBDEBUG( "__HCD_Addr_Obtain        : Using Address #%lu", adr );

			hn->hn_Function_Array[ adr ]  = fn;
//			hn->hn_EndPoint_Array[ adr * 32 +  0 ] = fkt->fkt_ControlEndPoint;
//			hn->hn_EndPoint_Array[ adr * 32 + 16 ] = fkt->fkt_ControlEndPoint;
			retval = adr;
			break;
		}
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Addr_Semaphore );

	if ( adr >= 128 )
	{
		USBDEBUG( "__HCD_Addr_Obtain : No free Address" );
	}
	else
	{
		USBDEBUG( "__HCD_Addr_Obtain        : New Address #%lu allocated", adr );
	}

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
