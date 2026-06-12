
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
#include "OHCI.h"

// --

static S32 __is_ED_Finished( struct USB2_HCDNode *hn UNUSED, struct OHCI_ED *ed )
{
struct OHCI_TD *td;
S32 retval;
U32 status;
U32 len;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "OHCI : __is_ED_Finished" );

	retval = TRUE;

	td = ed->ped_first_td;

	while( td )
	{
		status = OHCI_TD_GET_CC( LE_SWAP32( td->td_flags ));

		if ( status == 15 )
		{
			// Still Active
			retval = FALSE;
			break;
		}

		if ( status )
		{
			// An error
			switch( status )
			{
				case /* 0 */ OHCI_CC_NO_ERROR:
				{
					USBDEBUG( "No Error, check next TD" );
					break;
				}

				case /* 1 */ OHCI_CC_CRC:
				{
					USBINFO( "OHCI_CC_CRC" );
					break;
				}

				case /* 2 */ OHCI_CC_BIT_STUFFING:
				{
					USBINFO( "OHCI_CC_BIT_STUFFING" );
					break;
				}

				case /* 3 */ OHCI_CC_DATA_TOGGLE_MISMATCH:
				{
					USBINFO( "OHCI_CC_DATA_TOGGLE_MISMATCH" );
					break;
				}

				case /* 4 */ OHCI_CC_STALL:
				{
					USBINFO( "OHCI_CC_STALL" );
					break;
				}

				case /* 5 */ OHCI_CC_DEVICE_NOT_RESPONDING:
				{
					USBINFO( "OHCI_CC_DEVICE_NOT_RESPONDING" );
					break;
				}

				case /* 6 */ OHCI_CC_PID_CHECK_FAILURE:
				{
					USBINFO( "OHCI_CC_PID_CHECK_FAILURE" );
					break;
				}

				case /* 7 */ OHCI_CC_UNEXPECTED_PID:
				{
					USBINFO( "OHCI_CC_UNEXPECTED_PID" );
					break;
				}

				case /* 8 */ OHCI_CC_DATA_OVERRUN:
				{
					USBINFO( "OHCI_CC_DATA_OVERRUN" );
					break;
				}

				case /* 9 */ OHCI_CC_DATA_UNDERRUN:
				{
					USBINFO( "OHCI_CC_DATA_UNDERRUN" );
					break;
				}

				/* A * Reserved / Unused */

				/* B * Reserved / Unused */

				case /* C */ OHCI_CC_BUFFER_OVERRUN:
				{
					USBINFO( "OHCI_CC_BUFFER_OVERRUN" );
					break;
				}

				case /* D */ OHCI_CC_BUFFER_UNDERRUN:
				{
					USBINFO( "OHCI_CC_BUFFER_UNDERRUN" );
					break;
				}

				/* E * Not Accessed / Still Active */
				/* F * Not Accessed / Still Active */

				default:
				{
					USBINFO( "Unknown error ( %02x )", status );
					break;
				}
			}
			break;
		}

		if ( td->ptd_buffer_virt )
		{
			if ( td->td_cbp )
			{
				len = LE_SWAP32( td->td_be ) - LE_SWAP32( td->td_cbp ) + 1;
			}
			else
			{
				len = LE_SWAP32( td->td_be )  - td->ptd_buffer_phy + 1;
			}

			if ( len < 2 * 4096 )
			{
				break;
			}
		}

		td = td->ptd_virt_next;
	}

	TASK_NAME_LEAVE();

	return( retval );
}

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

SEC_CODE S32 OHCI_Transfer_Check( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct OHCI_ED *ed;
S32 retval;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "OHCI : OHCI_Transfer_Check" );

	ed = ioreq->req_HCD.OHCI.ED_Header;

	if ( ed )
	{
		retval = __is_ED_Finished( hn, ed );
	}
	else
	{
		USBERROR( "OHCI_Transfer_Check : NULL Pointer" );
		retval = FALSE;
	}

	TASK_NAME_LEAVE();

	return( retval );
}

// --
