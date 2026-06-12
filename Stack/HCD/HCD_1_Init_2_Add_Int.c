
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Init_1_Add_Int( struct USBBase *usbbase, struct USB3_HCDNode *hn )
{
S32 retval;

	retval = FALSE;

	USBDEBUG( "__Init_1_Add_Int" );

	// --
	// Setup interrupt

	if ( hn->HCD_Functions.Handler_Interrupt )
	{
		USBINFO( "Setting up USB HCD Interrupt %ld at %p", hn->hn_HCDInterruptNr, hn->HCD_Functions.Handler_Interrupt );

		hn->HCD_Interrupt.is_Node.ln_Succ		= NULL;
		hn->HCD_Interrupt.is_Node.ln_Pred		= NULL;
		hn->HCD_Interrupt.is_Node.ln_Type		= NT_EXTINTERRUPT;
		hn->HCD_Interrupt.is_Node.ln_Pri		= 10;
		hn->HCD_Interrupt.is_Node.ln_Name		= (STR) "USB Host Controller Interrupt Handler";
		hn->HCD_Interrupt.is_Data				= hn;
		hn->HCD_Interrupt.is_Code				= (PTR) hn->HCD_Functions.Handler_Interrupt;

		if ( ! MISC_ADDINTSERVER( hn->hn_HCDInterruptNr, & hn->HCD_Interrupt ))
		{
			USBERROR( "Error Adding USB Interrupt Server" );
			goto bailout;
		}

		hn->HCD_Interrupt_Added = TRUE;
	}

	// --
	// Adds a reset handler which stops the controller before reset

	if ( hn->HCD_Functions.Handler_Reset )
	{
		USBINFO( "Adding Reset Handler at %p", hn->HCD_Functions.Handler_Interrupt );

		hn->hn_Reset_Interrupt.is_Node.ln_Succ	= NULL;
		hn->hn_Reset_Interrupt.is_Node.ln_Pred	= NULL;
		hn->hn_Reset_Interrupt.is_Node.ln_Type	= NT_EXTINTERRUPT;
		hn->hn_Reset_Interrupt.is_Node.ln_Pri	= 10;
		hn->hn_Reset_Interrupt.is_Node.ln_Name	= (STR) "USB Host Controller Reset Callback";
		hn->hn_Reset_Interrupt.is_Data			= hn;
		hn->hn_Reset_Interrupt.is_Code			= (PTR) hn->HCD_Functions.Handler_Reset;

		if ( ! MISC_ADDRESETCALLBACK( & hn->hn_Reset_Interrupt ))
		{
			USBERROR( "Error Adding Reset Interrupt" );
			goto bailout;
		}

		hn->hn_Reset_Interrupt_Added = TRUE;
	}

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --
