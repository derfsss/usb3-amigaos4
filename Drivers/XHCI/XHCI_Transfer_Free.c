
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"
#include "XHCI.h"

// --

SEC_CODE void XHCI_Transfer_Free( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
	struct USBBase *usbbase = hn->hn_USBBase;

	if ( ioreq->req_HCD.XHCI.DataBuffer )
	{
		MEMORY_FREE( MEMID_HCD_20k, ioreq->req_HCD.XHCI.DataBuffer, ioreq->req_HCD.XHCI.DataBufferPhy );
		ioreq->req_HCD.XHCI.DataBuffer = NULL;
	}
}

// --
