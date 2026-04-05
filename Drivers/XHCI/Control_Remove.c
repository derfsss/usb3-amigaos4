
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

SEC_CODE void XHCI_Control_Remove( struct USB2_HCDNode *hn UNUSED, struct RealRequest *ioreq )
{
	// XHCI TRBs are consumed by the controller — nothing to unlink
	ioreq->req_PublicStat = IORS_HCD;
}

// --
