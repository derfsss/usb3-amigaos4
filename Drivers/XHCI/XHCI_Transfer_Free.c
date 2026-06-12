
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

	// Single bounce buffer (control / interrupt). DataBufferPool records
	// which pool it came from -- control uses MEMID_HCD_20k, interrupt
	// MEMID_HCD_4k. (Freeing into the wrong pool poisons that pool's
	// free list with wrong-sized chunks.)

	if ( ioreq->req_HCD.XHCI.DataBuffer )
	{
		MEMORY_FREE( ioreq->req_HCD.XHCI.DataBufferPool,
			ioreq->req_HCD.XHCI.DataBuffer, ioreq->req_HCD.XHCI.DataBufferPhy );
		ioreq->req_HCD.XHCI.DataBuffer = NULL;
	}

	// Bulk chunk list + chunk table

	if ( ioreq->req_HCD.XHCI.BounceTable )
	{
		struct XHCI_BounceChunk *chunks = ioreq->req_HCD.XHCI.BounceTable;
		U32 cnt;

		for ( cnt = 0 ; cnt < ioreq->req_HCD.XHCI.BounceCnt ; cnt++ )
		{
			if ( chunks[ cnt ].Virt )
			{
				MEMORY_FREE( MEMID_HCD_20k, chunks[ cnt ].Virt, chunks[ cnt ].Phy );
			}
		}

		MEMORY_FREE( MEMID_HCD_4k, chunks, ioreq->req_HCD.XHCI.BounceTablePhy );

		ioreq->req_HCD.XHCI.BounceTable = NULL;
		ioreq->req_HCD.XHCI.BounceCnt   = 0;
	}
}

// --
