
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

// Stub -- will be implemented in Phase 3c/3d/3e

SEC_CODE S32 XHCI_Bulk_Build( struct USB2_HCDNode *hn UNUSED, struct RealRequest *ioreq UNUSED )
{
	return( TRUE );
}

// --
