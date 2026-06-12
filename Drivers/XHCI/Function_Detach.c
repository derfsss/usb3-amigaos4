
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
//
// This runs in the CALLER's context (typically the hub driver's task,
// holding the stack's lock semaphore), so it must not block: issuing a
// Disable Slot command and waiting for its completion here deadlocks
// against the HCD task (proven on hardware, June 2026). Instead the
// address mappings are cleared immediately and the slot is queued for
// disposal; XHCI_Handler_HCD drains the queue in the HCD task context.
//
// Without this cleanup, every unplug leaked a slot (MaxSlots is 32 on
// the uPD720202) and left a stale SlotID_ByAddress entry that a later
// device on the same address would inherit.

SEC_CODE void XHCI_Function_Detach( struct USB3_HCDNode *hn, struct RealFunctionNode *fn )
{
struct _XHCI *xhci;
struct XHCI_Slot *slot;
U32 slotid;
U32 adr;
U32 port;

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

	if (( slotid ) && ( slotid <= xhci->MaxSlots ))
	{
		slot = xhci->Slots[ slotid ];

		xhci->SlotID_ByAddress[ adr ] = 0;

		if ( slot )
		{
			if ( PCI_READLONG( xhci->OpBase + XHCI_USBSTS ) & XHCI_STS_HCH )
			{
				// Controller halted: no commands possible or needed,
				// the rings can be freed directly
				XHCI_Slot_Free( hn, slotid );
			}
			else
			{
				// Queue for the HCD task; the Signal makes the drain
				// run promptly (the 1 s HCD tick is the fallback)
				slot->PendingDisable = 1;

				if ( xhci->HCDTask )
				{
					usbbase->usb_IExec->Signal( xhci->HCDTask,
						xhci->Signal_Event.sig_Signal_Mask );
				}
			}
		}
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
