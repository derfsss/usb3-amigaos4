
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "XHCI.h"

// --

SEC_CODE U32 XHCI_Control_Length( struct USB2_HCDNode *hn UNUSED, struct RealRequest *ioreq )
{
U32 actual;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Control_Length" );

	actual = 0;

	if ( ioreq->req_HCD.XHCI.DataBuffer && ioreq->req_HCD.XHCI.DataBufferLen )
	{
		actual = ioreq->req_HCD.XHCI.DataBufferLen;

		// For CMD_READ (IN): copy from DMA bounce buffer to user buffer
		if (( ioreq->req_Public.io_Command == CMD_READ ) && ( ioreq->req_Public.io_Data ))
		{
			MEM_COPY( ioreq->req_HCD.XHCI.DataBuffer, ioreq->req_Public.io_Data, actual );

			usbbase->usb_IExec->DebugPrintF( "XHCI: Control_Length: copied %ld bytes (cc=%ld)\n",
				actual, ioreq->req_HCD.XHCI.CompletionCode );
		}
	}

	TASK_NAME_LEAVE();

	return( actual );
}

// --
