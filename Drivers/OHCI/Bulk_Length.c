
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

/*
** Should set io_Error if there are transfer errors
*/

SEC_CODE U32 OHCI_Bulk_Length( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct OHCI_ED *ed;
struct OHCI_TD *td;
U32 status;
U32 actual;
U32 bytes;
S32 copy;
U32 size;
PTR mem;
U32 len;
U32 CBP;
U32 BE;
U8 err;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Bulk_Length" );

	actual = 0;

	// --

	mem = ioreq->req_Public.io_Data;
	ed	= ioreq->req_HCD.OHCI.ED_Header;

	if ( ! ed )
	{
		goto bailout;
	}

	if (( ioreq->req_Public.io_Command == CMD_READ ) && ( mem ))
	{
		copy = TRUE;
	}
	else
	{
		copy = FALSE;
	}

	// -- Setup Data

	td = ed->ped_first_td;

	if ( ! td )
	{
		goto bailout;
	}

	status = OHCI_TD_GET_CC( LE_SWAP32( td->td_flags ) );

	/**/ if ( status == OHCI_CC_NOT_ACCESSED )
	{
		// Transfer not finished yet
		goto bailout;
	}
	else if ( status != OHCI_CC_NO_ERROR )
	{
		switch( status )
		{
			case OHCI_CC_STALL:
			{
				err = USB3Err_Host_Stall;
				break;
			}

			case OHCI_CC_CRC:
			case OHCI_CC_BIT_STUFFING:
			case OHCI_CC_PID_CHECK_FAILURE:
			{
				err = USB3Err_Host_CRCError;
				break;
			}

			case OHCI_CC_DEVICE_NOT_RESPONDING:
			{
				err = USB3Err_Host_Timeout;
				break;
			}

			case OHCI_CC_DATA_OVERRUN:
			case OHCI_CC_BUFFER_OVERRUN:
			{
				err = USB3Err_Host_Overflow;
				break;
			}

			case OHCI_CC_DATA_UNDERRUN:
			case OHCI_CC_BUFFER_UNDERRUN:
			{
				err = USB3Err_Host_RuntPacket;
				break;
			}

			case OHCI_CC_UNEXPECTED_PID:
			case OHCI_CC_DATA_TOGGLE_MISMATCH:
			{
				err = USB3Err_Host_HostError;
				break;
			}
	
			default:
			{
				err = USB3Err_Host_UnknownError;
				break;
			}
		}

		ioreq->req_Public.io_Error = err;
		goto bailout;
	}

	// Skip first Setup TD
	td = td->ptd_virt_next;

	// --
	// run all execpt last one

	while( td )
	{
		if ( ! td->ptd_buffer_virt )
		{
			break;
		}

		BE  	= LE_SWAP32( td->td_be );
		CBP		= LE_SWAP32( td->td_cbp );
		status	= OHCI_TD_GET_CC( LE_SWAP32( td->td_flags ) );
 		size	= td->ptd_buffer_size;

		/**/ if ( CBP == 0 )
		{
			bytes = 0;
		}
		else if ( BE >= CBP )
		{
			bytes = BE - CBP + 1;
		}
		else
		{
			bytes = 0;
		}

 		len	= ( size >= bytes ) ? ( size - bytes ) : ( 0 ) ;

		/**/ if ( status == OHCI_CC_NOT_ACCESSED )
		{
			break;
		}
		else if (( status != OHCI_CC_NO_ERROR ) && ( status != OHCI_CC_DATA_UNDERRUN ))
		{
			// treat 'OHCI_CC_DATA_UNDERRUN' as normal completion, correct for Control/Bulk

			switch( status )
			{
				case OHCI_CC_STALL:
				{
					err = USB3Err_Host_Stall;
					break;
				}

				case OHCI_CC_CRC:
				case OHCI_CC_BIT_STUFFING:
				case OHCI_CC_PID_CHECK_FAILURE:
				{
					err = USB3Err_Host_CRCError;
					break;
				}
	
				case OHCI_CC_DEVICE_NOT_RESPONDING:
				{
					err = USB3Err_Host_Timeout;
					break;
				}
	
				case OHCI_CC_DATA_OVERRUN:
				case OHCI_CC_BUFFER_OVERRUN:
				{
					err = USB3Err_Host_Overflow;
					break;
				}

				case OHCI_CC_BUFFER_UNDERRUN:
				{
					err = USB3Err_Host_RuntPacket;
					break;
				}
	
				case OHCI_CC_UNEXPECTED_PID:
				case OHCI_CC_DATA_TOGGLE_MISMATCH:
				{
					err = USB3Err_Host_HostError;
					break;
				}
	
				default:
				{
					err = USB3Err_Host_UnknownError;
					break;
				}
			}

			ioreq->req_Public.io_Error = err;
			goto bailout;
		}

		if (( copy ) && ( len ))
		{
			// copy src, dest, len
			MEM_COPY( td->ptd_buffer_virt, mem, len );
		}

		actual	+= len;
		mem		+= len;

		// Check for short Packet
		if ( len < size )
		{
			break;
		}

		td = td->ptd_virt_next;
	}

	// --

bailout:

	TASK_NAME_LEAVE();

	return( actual );
}

// --
