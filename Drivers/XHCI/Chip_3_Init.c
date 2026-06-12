
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

SEC_CODE S32 XHCI_Chip_Init( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
struct XHCI_TRB *trb;
U32 cnt;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Init" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// -- Zero address-to-slot mapping

	MEM_SET( xhci->SlotID_ByAddress, 0, 128 );

	// -- Zero DCBAA

	usbbase->usb_IExec->DebugPrintF( "XHCI: Init : 1 : Zero DCBAA\n" );

	MEM_SET( xhci->DCBAA, 0, 4096 );

	// -- Initialize Command Ring

	usbbase->usb_IExec->DebugPrintF( "XHCI: Init : 2 : Command Ring\n" );

	MEM_SET( xhci->CmdRing.trbs, 0, 4096 );

	// Place Link TRB at last position -- points back to ring start, toggles cycle
	trb = & xhci->CmdRing.trbs[ xhci->CmdRing.size - 1 ];
	trb->trb_param_lo = LE_SWAP32( xhci->CmdRing.phys );
	trb->trb_param_hi = 0;
	trb->trb_status   = 0;
	trb->trb_control  = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_LINK ) | XHCI_TRB_TC );

	// -- Initialize Event Ring

	usbbase->usb_IExec->DebugPrintF( "XHCI: Init : 3 : Event Ring\n" );

	MEM_SET( xhci->EvtRing.trbs, 0, 4096 );

	// -- Fill Event Ring Segment Table (1 segment)

	usbbase->usb_IExec->DebugPrintF( "XHCI: Init : 4 : ERST\n" );

	MEM_SET( xhci->ERST, 0, 4096 );

	xhci->ERST[0].erste_addr_lo = LE_SWAP32( xhci->EvtRing.phys );
	xhci->ERST[0].erste_addr_hi = 0;
	xhci->ERST[0].erste_size    = LE_SWAP32( xhci->EvtRing.size );
	xhci->ERST[0].erste_reserved = 0;

	// -- Allocate Scratchpad buffers if needed

	if ( xhci->ScratchpadCount > 0 )
	{
		usbbase->usb_IExec->DebugPrintF( "XHCI: Init : 5 : Scratchpad (%ld pages)\n", xhci->ScratchpadCount );

		// Scratchpad array -- array of 64-bit physical addresses
		xhci->ScratchpadArray = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

		if ( ! xhci->ScratchpadArray )
		{
			USBERROR( "XHCI: Error allocating scratchpad array" );
			goto bailout;
		}

		xhci->ScratchpadArray_Phyaddr = ((struct Mem_FreeNode *) xhci->ScratchpadArray)->mfn_Addr;

		MEM_SET( xhci->ScratchpadArray, 0, 4096 );

		// Allocate tracking arrays
		xhci->ScratchpadPages = MEM_ALLOCVEC( sizeof( PTR ) * xhci->ScratchpadCount, TRUE );

		if ( ! xhci->ScratchpadPages )
		{
			USBERROR( "XHCI: Error allocating scratchpad page tracking" );
			goto bailout;
		}

		xhci->ScratchpadCount2 = xhci->ScratchpadCount;

		// Allocate individual scratchpad pages
		for ( cnt = 0 ; cnt < xhci->ScratchpadCount ; cnt++ )
		{
			PTR page = MEMORY_ALLOC( MEMID_HCD_4k, FALSE );

			if ( ! page )
			{
				USBERROR( "XHCI: Error allocating scratchpad page %ld", cnt );
				goto bailout;
			}

			xhci->ScratchpadPages[cnt] = page;

			U32 page_phy = ((struct Mem_FreeNode *) page)->mfn_Addr;

			// Store physical address in scratchpad array (64-bit LE)
			xhci->ScratchpadArray[ cnt * 2 ]     = LE_SWAP32( page_phy );
			xhci->ScratchpadArray[ cnt * 2 + 1 ] = 0;
		}

		// Point DCBAA[0] to scratchpad array
		xhci->DCBAA[0] = LE_SWAP32( xhci->ScratchpadArray_Phyaddr );
		xhci->DCBAA[1] = 0;
	}

	// -- Program the controller registers

	usbbase->usb_IExec->DebugPrintF( "XHCI: Init : 6 : Program registers\n" );

	// Max Device Slots Enabled
	PCI_WRITELONG( xhci->OpBase + XHCI_CONFIG, xhci->MaxSlots );

	// Device Context Base Address Array Pointer
	PCI_WRITELONG( xhci->OpBase + XHCI_DCBAAP_LO, xhci->DCBAA_Phyaddr );
	PCI_WRITELONG( xhci->OpBase + XHCI_DCBAAP_HI, 0 );

	// Command Ring Control Register (physical addr with cycle bit)
	PCI_WRITELONG( xhci->OpBase + XHCI_CRCR_LO, xhci->CmdRing.phys | XHCI_CRCR_RCS );
	PCI_WRITELONG( xhci->OpBase + XHCI_CRCR_HI, 0 );

	// Event Ring Segment Table Size (interrupter 0)
	PCI_WRITELONG( xhci->RunBase + XHCI_ERSTSZ(0), 1 );

	// Event Ring Dequeue Pointer (interrupter 0)
	PCI_WRITELONG( xhci->RunBase + XHCI_ERDP_LO(0), xhci->EvtRing.phys | XHCI_ERDP_EHB );
	PCI_WRITELONG( xhci->RunBase + XHCI_ERDP_HI(0), 0 );

	// Event Ring Segment Table Base Address (interrupter 0)
	// Must be written AFTER ERSTSZ per XHCI spec
	PCI_WRITELONG( xhci->RunBase + XHCI_ERSTBA_LO(0), xhci->ERST_Phyaddr );
	PCI_WRITELONG( xhci->RunBase + XHCI_ERSTBA_HI(0), 0 );

	usbbase->usb_IExec->DebugPrintF( "XHCI: Init : Done\n" );

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
