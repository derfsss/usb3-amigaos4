
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

SEC_CODE void XHCI_Interrupt_Remove( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
	// If the transfer completed, its TRBs were consumed by the
	// controller and there is nothing to unlink. Otherwise (AbortIO,
	// timeout, device detach) the hardware still owns the TD and must
	// be stopped and flushed first.
	XHCI_Transfer_Abort( hn, ioreq );

	ioreq->req_PublicStat = IORS_HCD;
}

// --
