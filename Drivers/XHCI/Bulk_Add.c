
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

SEC_CODE void XHCI_Bulk_Add( struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct _XHCI *xhci;
U32 slotid;
U32 dci;

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	struct USBBase *usbbase = hn->hn_USBBase;
	#endif

	TASK_NAME_ENTER( "XHCI : XHCI_Bulk_Add" );

	xhci   = & hn->hn_HCD.XHCI;
	slotid = ioreq->req_HCD.XHCI.SlotID;
	dci    = ioreq->req_HCD.XHCI.DCI;

	// Ring the endpoint doorbell
	PCI_WRITELONG( xhci->DoorbellBase + 4 * slotid, dci );

	ioreq->req_PublicStat = IORS_HCD_Active;

	TASK_NAME_LEAVE();
}

// --
