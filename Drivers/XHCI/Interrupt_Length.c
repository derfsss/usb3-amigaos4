
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

SEC_CODE U32 XHCI_Interrupt_Length( struct USB3_HCDNode *hn UNUSED, struct RealRequest *ioreq )
{
U32 actual;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Interrupt_Length" );

	actual = 0;

	if ( ioreq->req_HCD.XHCI.DataBuffer && ioreq->req_HCD.XHCI.DataBufferLen )
	{
		actual = ioreq->req_HCD.XHCI.DataBufferLen - ioreq->req_HCD.XHCI.Residual;

		// For CMD_READ (IN): copy from DMA bounce buffer to user buffer
		if (( ioreq->req_Public.io_Command == CMD_READ ) && ( ioreq->req_Public.io_Data ))
		{
			MEM_COPY( ioreq->req_HCD.XHCI.DataBuffer, ioreq->req_Public.io_Data, actual );
		}
	}

	TASK_NAME_LEAVE();

	return( actual );
}

// --
