
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) at
 * Carlstedt Research & Technology.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// --

#include "usb3_all.h"

#include "HUB.h"

// --

SEC_CODE void HUB_Handle_Interrupt( struct USBBase *usbbase, struct intern *in )
{
struct USB3_IORequest *ioreq;
struct ExecIFace *IExec;
U32 port;
U32 max;
U8 *data;

	USBDEBUG( "HUB : Handle Interrupt" );

	max = 5;

	IExec = usbbase->usb_IExec;

	IExec->DebugPrintF( "USB3: HUB_Interrupt: enter (ports=%ld tier=%ld)\n", in->Ports, in->Function->fkt_Tier );

	while( max-- )
	{
		ioreq = (PTR) IExec->GetMsg( in->Res_Interrupt->MsgPort );

		if ( ! ioreq )
		{
			break;
		}

		switch( ioreq->io_Error )
		{
			case USB3Err_NoError:
			{
				IExec->DebugPrintF( "USB3: HUB_Interrupt: OK actual=%ld\n", ioreq->io_Actual );

				in->ErrorCnt = 0;

				if (( in->Ports+7 ) / 8 <= ioreq->io_Actual )
				{
					// Something changed, check all ports
					data = ioreq->io_Data;

					for( port=0 ; port <= in->Ports ; port++ )
					{
						if ( data[ port / 8 ] & ( 1UL << ( port & 0x07 )))
						{
							if ( port == 0 )
							{
								USBDEBUG( "HUB : Int : HUB %lu Status Change", port );

								HUB__HUB_Chg( usbbase, in );
							}
							else
							{
								IExec->DebugPrintF( "USB3: HUB_Interrupt: Port %ld change!\n", port );

								HUB__Port_Chg( usbbase, in, port );
							}
						}
					}
				}
				break;
			}

			case USB3Err_Device_Detached:
			{
				USBDEBUG( "HUB : Int : Detached" );
				in->Running = FALSE;
				break;
			}

			default:
			{
				if ( in->ErrorCnt++ >= 20 )
				{
					USBDEBUG( "HUB : Int : Too many errors, quitting" );
					in->Running = FALSE;
				}
				else
				{
					USBDEBUG( "HUB : Int : Unknown IOError %ld (ErrCnt %ld)", ioreq->io_Error, in->ErrorCnt );
				}
				break;
			}
		}

		if ( in->Running )
		{
			// We only have 1 IORequest.. so no need to pass ioreq to HUB_Schedule
			HUB_Schedule( usbbase, in );
		}
		else
		{
			break;
		}
	}
}

// --
