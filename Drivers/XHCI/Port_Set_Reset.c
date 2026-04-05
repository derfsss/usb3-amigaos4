
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

SEC_CODE S32 XHCI_Port_Set_Reset( struct USB2_HCDNode *hn, U32 port )
{
struct _XHCI *xhci;
U32 val;
U32 cnt;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Port_Set_Reset" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// Set Port Reset bit (preserve non-change bits, clear change bits)
	val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );
	val &= ~XHCI_PS_CHANGE_MASK;
	val |= XHCI_PS_PR;
	PCI_WRITELONG( xhci->OpBase + XHCI_PORTSC( port - 1 ), val );

	// Wait for Port Reset Change
	for ( cnt = 0 ; cnt < 500 ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );

		val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );

		if ( val & XHCI_PS_PRC )
		{
			break;
		}
	}

	if ( ! ( val & XHCI_PS_PRC ) )
	{
		USBERROR( "XHCI: Port %ld reset timeout", port );
		goto bailout;
	}

	// Clear PRC (write 1 to clear, preserve other bits)
	val &= ~XHCI_PS_CHANGE_MASK;
	val |= XHCI_PS_PRC;
	PCI_WRITELONG( xhci->OpBase + XHCI_PORTSC( port - 1 ), val );

	// Mark software reset change
	xhci->PortResetChange[ port ] = 1;

	// Read back to check port is enabled
	val = PCI_READLONG( xhci->OpBase + XHCI_PORTSC( port - 1 ) );

	if ( val & XHCI_PS_PED )
	{
		U32 speed = ( val & XHCI_PS_SPEED_MASK ) >> XHCI_PS_SPEED_SHIFT;
		U32 usb_speed;

		// Map XHCI speed to USB speed enum
		switch( speed )
		{
			case XHCI_PS_SPEED_FS: usb_speed = USBSPEED_Full; break;
			case XHCI_PS_SPEED_LS: usb_speed = USBSPEED_Low; break;
			case XHCI_PS_SPEED_HS: usb_speed = USBSPEED_High; break;
			case XHCI_PS_SPEED_SS: usb_speed = USBSPEED_Super; break;
			default:               usb_speed = USBSPEED_Full; break;
		}

		usbbase->usb_IExec->DebugPrintF( "XHCI: Port %ld reset complete, speed=%ld\n",
			port, speed );

		// XHCI requires Enable Slot + Address Device (BSR=1) immediately
		// after port reset, before any control transfers can happen.
		// This sets up the slot and EP0 without assigning a USB address.
		{
			U32 slotid = XHCI_Cmd_EnableSlot( hn );

			if ( slotid )
			{
				if ( XHCI_Slot_Alloc( hn, slotid, port - 1, usb_speed ) )
				{
					if ( XHCI_Cmd_AddressDevice( hn, slotid, 1 ) )	// BSR=1
					{
						// Store slot at address 0 for initial control transfers
						xhci->SlotID_ByAddress[0] = (U8) slotid;

						usbbase->usb_IExec->DebugPrintF( "XHCI: Port %ld slot %ld ready (BSR mode)\n",
							port, slotid );
					}
					else
					{
						USBERROR( "XHCI: Port %ld Address Device BSR failed", port );
						XHCI_Slot_Free( hn, slotid );
						XHCI_Cmd_DisableSlot( hn, slotid );
					}
				}
				else
				{
					XHCI_Cmd_DisableSlot( hn, slotid );
				}
			}
		}

		retval = USB2Err_NoError;
	}
	else
	{
		USBERROR( "XHCI: Port %ld not enabled after reset", port );
	}

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
