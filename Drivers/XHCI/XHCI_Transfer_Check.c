
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

SEC_CODE S32 XHCI_Transfer_Check( struct USB2_HCDNode *hn UNUSED, struct RealRequest *ioreq )
{
	return( ioreq->req_HCD.XHCI.Completed != 0 ) ? TRUE : FALSE;
}

// --
