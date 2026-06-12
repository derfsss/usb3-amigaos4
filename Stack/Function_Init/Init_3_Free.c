
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#if 0
static void __myFree( struct USBBase *usbbase UNUSED, PTR userdata UNUSED, PTR in UNUSED )
{
//struct RealFunctionNode *fn;
//struct USB3_HCDNode *hn;

	USBDEBUG( "__myInit (Free) : Enter : DN    %p", dn );

	// --

	if ( DRIVER_VALID( dn ) != VSTAT_Okay )
	{
		USBDEBUG( "__myInit : Invalid DN (%p)", dn );
		goto bailout;
	}

	// --

//	hn = dn->dn_HCD;
//
//	if ( HCD_VALID( hn ) != VSTAT_Okay )
//	{
//		USBDEBUG( "__myInit: Invalid HCD (%p)", hn );
//		goto bailout;
//	}

	// --

	USBDEBUG( "__myFree (Init) : Done" );

bailout:

	// -- Exit

	USBDEBUG( "__myFree (Init) :: Leave" );

	return;
}
#endif

// --
