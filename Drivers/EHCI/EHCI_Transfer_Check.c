
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

SEC_CODE static S32 __is_QH_Finished( struct USB3_HCDNode *hn UNUSED, struct EHCI_QH *qh )
{
struct EHCI_TD *td;
S32 retval;
U32 status;
U32 len;
U32 l;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "EHCI : __is_QH_Finished" );

	retval = TRUE;

	if ( retval )
	{
		// Check QH

		status = LE_SWAP32( qh->qh_status );

		/**/ if ( status & EHCI_TD_ACTIVE )
		{
			// Still active, nothing to be done
			retval = FALSE;
		}
		else if ( status & EHCI_TD_STATERRS )
		{
			// Error - We are done
			// Errors handled else were
		}
	}

	if ( retval )
	{
		// Check TDs

		td = qh->pqh_first_td;

		while( td )
		{
			status = LE_SWAP32( td->td_status );

			len = EHCI_TD_GET_BYTES( status );

			if ( status & EHCI_TD_ACTIVE )
			{
				// Still active, nothing to be done
				retval = FALSE;
				break;
			}

			if ( status & EHCI_TD_STATERRS )
			{
				// Error - We are done
				// Errors handled else were
				break;
			}

			if ( ! td->td_buffer[1] )
			{
				l = 4*1024;
			}
			else
			{
				l = 20*1024;
			}

			if (( len != l ) && ( len != 0 ))
			{
				// Short packet - We are done
				break;
			}

			td = td->ptd_virt_next;
		}
	}

	TASK_NAME_LEAVE();

	return( retval );
}

#if 0
#if 0
// hn->hn_USBBase->usb_IExec->Disable();

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> qh : %p\n", qh );

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Status : %08lx\n", status );

// if ( status & EHCI_QTD_ACTIVE )
// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Active\n" );

// if ( status & EHCI_TD_HALTED )
// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Halted\n" );

// if ( status & EHCI_QTD_BUFERR )
// hn->hn_USBBase->usb_IExec->DebugPrintF( "> BufErr\n" );

// if ( status & EHCI_QTD_BABBLE )
// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Babble\n" );

// if ( status & EHCI_QTD_XACTERR )
// hn->hn_USBBase->usb_IExec->DebugPrintF( "> XactErr\n" );

// if ( status & EHCI_QTD_MISSEDMICRO )
// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Missed Micro\n" );

// if ( status & EHCI_QTD_PINGSTATE )
// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Ping State\n" );

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> PID : %x\n", EHCI_QTD_GET_PID( status ) );

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> CErr : %x\n", EHCI_QTD_GET_CERR( status ) );

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> CPage : %x\n", EHCI_QTD_GET_C_PAGE( status ) );

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> IOC : %x\n", status & EHCI_QTD_IOC );

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Len : %d\n", EHCI_QTD_GET_BYTES( status ) );

// hn->hn_USBBase->usb_IExec->DebugPrintF( "> Toggle : %d\n", EHCI_QTD_GET_TOGGLE( status ) );

// hn->hn_USBBase->usb_IExec->Enable();

			EHCI_Dump_QH( hn, qh, TRUE );
			#endif
#endif

// --

/*
\\
// Returns:
\\   TRUE  - if Transfer is done
//   FALSE - if Transfer need more time to finish
\\
// Note: Running in HCD context
\\
*/

SEC_CODE S32 EHCI_Transfer_Check( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct EHCI_QH *qh;
S32 retval;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "EHCI : EHCI_Transfer_Check" );

	qh = ioreq->req_HCD.EHCI.QH_Header;

	if ( qh )
	{
		retval = __is_QH_Finished( hn, qh );
	}
	else
	{
		USBERROR( "EHCI_Transfer_Check : NULL Pointer" );
		retval = FALSE;
	}

	TASK_NAME_LEAVE();

	return( retval );
}

// --
