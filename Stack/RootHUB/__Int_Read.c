
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static U32 __Interrupt_Read( struct USBBase *usbbase, struct USB2_HCDNode *hn, U8 *buffer, U32 buflen )
{
U32 changed;
S32 update;
U32 actual;
U32 status;
U32 loops;
U32 port;

	// Clear Buffer
	MEM_SET( buffer, 0, buflen );

	loops	= MIN( hn->hn_HUB_NumPorts, buflen*8 );
	actual	= 0;
	update	= FALSE;

	// Port 0 is RootHUB we skip that
	for( port=1 ; port <= loops ; port++ )
	{
		// Returns HUBF_xx flags
		if ( hn->HCD_Functions.Port_Get_Status )
		{
			status = hn->HCD_Functions.Port_Get_Status( hn, port );
		}
		else
		{
			status = 0;
		}

		changed = status &
			( HUBF_Chg_Connection
			| HUBF_Chg_Enable
			| HUBF_Chg_Suspend
			| HUBF_Chg_Over_Current
			| HUBF_Chg_Reset );

		if ( changed )
		{
			buffer[ port / 8 ] |= ( 1UL << ( port & 0x07 ));
			update = TRUE;
		}
	}

	if ( update == FALSE )
	{
		USBDEBUG( "__Interrupt_Read : Nothing has changed" );
	}
	else
	{
		actual = 1 + loops / 8;
	}

	return( actual );
}
