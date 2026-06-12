
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

/*
** Called just before the system resets/reboots (software reboot only --
** an MCU/hardware reset gives us no chance to run).
**
** Halting + resetting the controller here is CRITICAL on the Renesas
** uPD720202: if it is left RUNNING across a warm reboot it retains
** internal state and wedges with HCRST stuck, and no software path can
** recover it afterwards (all six PCI/PCIe tiers in Chip_1_Reset fail;
** even an ATX power-off doesn't clear it because the PCIe slot keeps
** 3.3Vaux from the standby rail). Prefer software reboots while the
** usb3 stack is running.
**
** Runs in reset-callback context: no waiting primitives, so the HCH
** poll below is a bounded busy-read.
*/

SEC_CODE U32 XHCI_Handler_Reset( struct ExceptionContext *Context UNUSED, struct ExecBase *SysBase UNUSED, PTR userData )
{
struct USB3_HCDNode *hn;
struct _XHCI *xhci;
U32 cnt;

	hn = (PTR) userData;

	if (( hn ) && ( hn->hn_HCD.XHCI.OpBase ))
	{
		xhci = & hn->hn_HCD.XHCI;

		// Halt: clear Run/Stop (and interrupt enables)
		PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, 0 );

		// Bounded busy-poll for HCHalted (spec allows up to 16 ms;
		// reads over PCIe are ~1 us each)
		for ( cnt = 0 ; cnt < 100000 ; cnt++ )
		{
			if ( PCI_READLONG( xhci->OpBase + XHCI_USBSTS ) & XHCI_STS_HCH )
			{
				break;
			}
		}

		// Reset the controller so it reboots into a clean state
		PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, XHCI_CMD_HCRST );
	}

	return( TRUE );
}

// --
