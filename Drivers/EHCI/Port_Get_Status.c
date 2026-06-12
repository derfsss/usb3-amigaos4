
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
** Copyright (c) 2001 The NetBSD Foundation, Inc.
** All rights reserved.
**
** This code is derived from software contributed to The NetBSD Foundation
** by Lennart Augustsson (lennart@augustsson.net).
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. All advertising materials mentioning features or use of this software
**    must display the following acknowledgement:
**        This product includes software developed by the NetBSD
**        Foundation, Inc. and its contributors.
** 4. Neither the name of The NetBSD Foundation nor the names of its
**    contributors may be used to endorse or promote products derived
**    from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
** ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

// --

#include "usb3_all.h"
#include "EHCI.h"

// --

/*
** Return status to RootHUB
*/

SEC_CODE U32 EHCI_Port_Get_Status( struct USB2_HCDNode *hn, U32 port )
{
U32 status;
U32 retval;

	retval = 0;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

//	USBDEBUG( "EHCI : PORT_Get_Status( Port %ld )", port );

	if (( port <= 0 ) || ( hn->hn_HUB_NumPorts < port ))
	{
		USBPANIC( "Invalid Port Number #%lu", port );
	}

	status = PCI_READLONG( hn->hn_HCD.EHCI.CapLength+EHCI_PORTSC( port ));

	USBDEBUG( "EHCI : PORT_Get_Status( Port #%lu, Status $%08lx )", port, status );

	#if 0
	if ( status & 2 )
	{
		hn->hn_USBBase->usb_IExec->DebugPrintF( "EHCI : Connect Change\n" );
	}
	#endif

	if ( status & 0x00002000 ) // Port Owner
	{
		// We don't own the port
		retval = HUBF_Status_Power;
	}
	else
	{
		retval = ( HUBF_Status_High_Speed
					//	Connect Status Change
		|	(( status & 0x00000002 ) ? HUBF_Chg_Connection : 0 )
					//	Port Enable/Disable Change
		|	(( status & 0x00000008 ) ? HUBF_Chg_Enable : 0 )
					//  Over-current Change
		|	(( status & 0x00000020 ) ? HUBF_Chg_Over_Current : 0 )
					// Port Reset Change
		|	(( hn->hn_HCD.EHCI.PortResetChange[port] ) ? HUBF_Chg_Reset : 0 )


					// Current Connect Status
		|	(( status & 0x00000001 ) ? HUBF_Status_Connection : 0 )
					// Port Enable/Disable Status
		|	(( status & 0x00000004 ) ? HUBF_Status_Enable : 0 )
					// Over Current Status
		|	(( status & 0x00000010 ) ? HUBF_Status_Over_Current : 0 )
					// Port Suspend Status
		|	(( status & 0x00000080 ) ? HUBF_Status_Suspend : 0 )
					// Port Reset Status
		|	(( status & 0x00000100 ) ? HUBF_Status_Reset : 0 )
					// Port Power Status
		|	(( status & 0x00001000 ) ? HUBF_Status_Power : 0 )
					// Port LowSpeed Status
		|	(( EHCI_PS_IS_LOWSPEED( status )) ? HUBF_Status_Low_Speed : 0 )
		);

		// Check for Software controlled LED Indicators
		if ( EHCI_HCS_P_INDICATOR( PCI_READLONG( EHCI_HCSPARAMS )))
		{
			retval |= HUBF_Status_Indicator;
		}
	}

	#if 1
	#ifdef DO_DEBUG

	USBDEBUG( "%s%s%s%s%s%s%s",
		( retval & HUBF_Status_Connection ) ? "Current Connect Status, " : "",
		( retval & HUBF_Status_Enable ) ? "Port Enable, " : "",
		( retval & HUBF_Status_Suspend ) ? "Port Suspend, " : "",
		( retval & HUBF_Status_Over_Current ) ? "Over Current Active, " : "",
		( retval & HUBF_Status_Reset ) ? "Port Reset, " : "",
		( retval & HUBF_Status_Power ) ? "Port Power, " : "",
		( retval & HUBF_Status_Low_Speed ) ? "Low Speed, " : ""
	);

	USBDEBUG( "%s%s%s%s%s",
		( retval & HUBF_Chg_Connection ) ? "Connect Change, " : "",
		( retval & HUBF_Chg_Enable ) ? "Port Enable Change, " : "",
		( retval & HUBF_Chg_Suspend ) ? "Port Suspend Change, " : "",
		( retval & HUBF_Chg_Over_Current ) ? "Over Current Change, " : "",
		( retval & HUBF_Chg_Reset ) ? "Port Reset Change" : ""
	);

	#endif
	#endif

	return( retval );
}

// --
