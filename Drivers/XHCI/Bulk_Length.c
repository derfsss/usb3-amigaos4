
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

SEC_CODE U32 XHCI_Bulk_Length( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct XHCI_BounceChunk *chunks;
U32 actual;
U32 remain;
U32 cnt;
U8 *dst;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Bulk_Length" );

	actual = 0;

	if (( ioreq->req_HCD.XHCI.DataBufferLen )
	&&	( ioreq->req_HCD.XHCI.Residual <= ioreq->req_HCD.XHCI.DataBufferLen ))
	{
		actual = ioreq->req_HCD.XHCI.DataBufferLen - ioreq->req_HCD.XHCI.Residual;
	}

	// For CMD_READ (IN): copy the received bytes from the bounce chunks
	// back to the user buffer

	chunks = ioreq->req_HCD.XHCI.BounceTable;

	if (( actual )
	&&	( chunks )
	&&	( ioreq->req_Public.io_Command == CMD_READ )
	&&	( ioreq->req_Public.io_Data ))
	{
		dst    = ioreq->req_Public.io_Data;
		remain = actual;

		for ( cnt = 0 ; cnt < ioreq->req_HCD.XHCI.BounceCnt && remain ; cnt++ )
		{
			U32 l = MIN( chunks[ cnt ].Len, remain );

			MEM_COPY( chunks[ cnt ].Virt, dst, l );

			dst    += l;
			remain -= l;
		}
	}

	TASK_NAME_LEAVE();

	return( actual );
}

// --
