
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
// Called by the stack (via __HCD_Addr_Release) when a function's USB
// address is recycled -- i.e. the device is gone or failed enumeration.
// Release the xHCI device slot that was bound to that address: Disable
// Slot halts all endpoint activity for the device per xHCI 4.6.4, then
// the rings/contexts can be freed and the address mapping cleared.
//
// Without this, every unplug leaked a slot (MaxSlots is 32 on the
// uPD720202) and left a stale SlotID_ByAddress entry that a later
// device on the same address would inherit.

SEC_CODE void XHCI_Function_Detach( struct USB3_HCDNode *hn, struct RealFunctionNode *fn )
{
struct _XHCI *xhci;
U32 slotid;
U32 adr;
U32 port;
U32 sts;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Function_Detach" );

	xhci = & hn->hn_HCD.XHCI;
	adr  = fn->fkt_Address;

	if (( ! xhci->OpBase ) || ( adr == 0 ) || ( adr >= 128 ))
	{
		goto bailout;
	}

	slotid = xhci->SlotID_ByAddress[ adr ];

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Function_Detach: addr=%ld slot=%ld port=%ld\n",
		adr, slotid, (U32) fn->fkt_PortNr );

	if ( slotid )
	{
		// Only talk to the chip if it is actually running -- on a dead
		// or stopped controller the command would just time out
		sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );

		if ( ! ( sts & XHCI_STS_HCH ) )
		{
			XHCI_Cmd_DisableSlot( hn, slotid );
		}

		XHCI_Slot_Free( hn, slotid );

		xhci->SlotID_ByAddress[ adr ] = 0;
	}

	// Drop the cached post-reset port speed -- the next device on this
	// port negotiates its own
	port = fn->fkt_PortNr;

	if ( port < ( sizeof( xhci->PortSpeed ) / sizeof( xhci->PortSpeed[0] ) ) )
	{
		xhci->PortSpeed[ port ] = 0;
	}

bailout:

	TASK_NAME_LEAVE();
}

// --
