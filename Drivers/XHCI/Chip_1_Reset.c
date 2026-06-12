
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

// Wait for HCRST to read back as 0. Returns ms elapsed.

static U32 _hcrst_wait( struct USB2_HCDNode *hn, U32 max_ms )
{
struct USBBase *usbbase = hn->hn_USBBase;
struct _XHCI *xhci = & hn->hn_HCD.XHCI;
U32 cnt;
U32 cmd;

	for ( cnt = 0 ; cnt < max_ms ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );
		cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
		if ( ! ( cmd & XHCI_CMD_HCRST ) )
		{
			return( cnt );
		}
	}
	return( max_ms );
}

// Tier 3: PCI D3hot -> D0 power-state cycle. On chips that reset their
// core on a D3->D0 transition this clears retained internal state after
// a warm reboot. Tested result on this uPD720202: the chip reports
// NoSoftReset=1 and ignores the transition -- kept in the ladder anyway
// since it is cheap and may help other silicon.

static void _pci_power_cycle( struct USB2_HCDNode *hn )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
struct USBBase *usbbase = hn->hn_USBBase;
struct PCICapability *cap;
U32 pmcsr_off;
U32 pmcsr;

	if ( ! pd )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: PCI power cycle: no pcidev\n" );
		return;
	}

	cap = pd->FindCapability( PCI_CAPABILITYID_PM );
	if ( ! cap )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: PCI power cycle: no PM capability\n" );
		return;
	}

	pmcsr_off = cap->CapOffset + 4;
	pmcsr = pd->ReadConfigLong( pmcsr_off );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: PCI power cycle: PMCSR offset=0x%lx, current=0x%08lx (state=%ld)\n",
		pmcsr_off, pmcsr, pmcsr & 0x3 );

	// Enter D3hot
	pd->WriteConfigLong( pmcsr_off, ( pmcsr & ~0x3UL ) | 0x3 );
	HCD_WAIT_MS( hn, 10 );

	// Return to D0 (clears state to 0)
	pd->WriteConfigLong( pmcsr_off, pmcsr & ~0x3UL );
	HCD_WAIT_MS( hn, 10 );

	pmcsr = pd->ReadConfigLong( pmcsr_off );
	usbbase->usb_IExec->DebugPrintF(
		"USB3: PCI power cycle: PMCSR after D3->D0 = 0x%08lx (state=%ld)\n",
		pmcsr, pmcsr & 0x3 );

	// Re-enable bus master + memory + I/O after D0 transition
	{
		U32 cmd = pd->ReadConfigWord( PCI_COMMAND );
		cmd |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
		pd->WriteConfigWord( PCI_COMMAND, cmd );
		usbbase->usb_IExec->DebugPrintF( "USB3: PCI power cycle: PCI_COMMAND restored to 0x%04lx\n", cmd );
	}
}

// Tier 4: PCIe Function-Level Reset (FLR). When a device supports it,
// FLR is processed by the PCIe core, not the xHCI logic, so it can work
// even when HCRST is stuck. We check DevCap bit 28 first and bail if the
// device does not advertise FLR. Tested result on this uPD720202 card:
// NOT FLR-capable (DevCap bit 28 = 0), so this tier is a no-op here.
//
// We also save+restore PCI config space because FLR returns the device
// to power-on state (BARs cleared, PCI_COMMAND cleared, etc).

static S32 _pci_flr( struct USB2_HCDNode *hn )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
struct USBBase *usbbase = hn->hn_USBBase;
struct PCICapability *cap;
U32 devcap;
U32 devctl;
U32 bars[6];
U32 cmd;
U32 cache_lat;
U32 irq_line;
U32 cnt;

	if ( ! pd )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: FLR: no pcidev\n" );
		return( FALSE );
	}

	cap = pd->FindCapability( PCI_CAPABILITYID_EXP );
	if ( ! cap )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: FLR: no PCIe capability\n" );
		return( FALSE );
	}

	// Device Capabilities Register at PCIe-cap + 0x04, bit 28 = FLR Capable
	devcap = pd->ReadConfigLong( cap->CapOffset + 0x04 );
	usbbase->usb_IExec->DebugPrintF(
		"USB3: FLR: PCIe cap at 0x%lx, DevCap=0x%08lx (FLR-capable=%ld)\n",
		cap->CapOffset, devcap, ( devcap >> 28 ) & 1 );

	if ( ! ( devcap & ( 1U << 28 ) ) )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: FLR: device does not advertise FLR\n" );
		return( FALSE );
	}

	// Save config-space state we need to restore after FLR
	for ( cnt = 0 ; cnt < 6 ; cnt++ )
	{
		bars[ cnt ] = pd->ReadConfigLong( PCI_BASE_ADDRESS_0 + ( cnt * 4 ) );
	}
	cmd        = pd->ReadConfigWord( PCI_COMMAND );
	cache_lat  = pd->ReadConfigWord( PCI_CACHE_LINE_SIZE );		// CacheLineSize | LatencyTimer
	irq_line   = pd->ReadConfigByte( PCI_INTERRUPT_LINE );		// Interrupt Line

	usbbase->usb_IExec->DebugPrintF(
		"USB3: FLR: saved BAR0=0x%08lx CMD=0x%04lx IRQ=%ld\n",
		bars[0], cmd, irq_line );

	// Trigger FLR: write bit 15 (Initiate Function Level Reset) of
	// Device Control Register at PCIe-cap + 0x08
	devctl = pd->ReadConfigWord( cap->CapOffset + 0x08 );
	pd->WriteConfigWord( cap->CapOffset + 0x08, (UWORD)( devctl | ( 1U << 15 ) ) );

	usbbase->usb_IExec->DebugPrintF( "USB3: FLR: FLR bit set, waiting 200 ms ...\n" );

	// PCIe spec: device must complete FLR within 100 ms; we give 200 to be safe
	HCD_WAIT_MS( hn, 200 );

	// Restore config-space state
	for ( cnt = 0 ; cnt < 6 ; cnt++ )
	{
		pd->WriteConfigLong( PCI_BASE_ADDRESS_0 + ( cnt * 4 ), bars[ cnt ] );
	}
	pd->WriteConfigWord( PCI_CACHE_LINE_SIZE, (UWORD)cache_lat );
	pd->WriteConfigByte( PCI_INTERRUPT_LINE, (UBYTE)irq_line );

	// Re-enable bus master + memory + I/O
	pd->WriteConfigWord( PCI_COMMAND,
		(UWORD)( cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER ) );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: FLR: config space restored, BAR0=0x%08lx CMD=0x%04lx\n",
		pd->ReadConfigLong( PCI_BASE_ADDRESS_0 ), pd->ReadConfigWord( PCI_COMMAND ) );

	return( TRUE );
}

// Tier 5: PCIe Secondary Bus Reset (SBR) on the upstream bridge. The
// device can refuse FLR (and ours does), but SBR is a bridge feature so
// the device itself can't decline it. We find the PCI-to-PCI bridge whose
// secondary bus number matches our device's bus, toggle bit 6 of its
// BridgeControl register, then wait for the PCIe link to re-train.
// Tested result on the X5000: the P5020 PEX root port accepts the bit
// (readback shows it set) but silently no-ops the actual reset.

static S32 _pci_sbr( struct USB2_HCDNode *hn )
{
struct USBBase *usbbase = hn->hn_USBBase;
struct PCIDevice *pd = hn->hn_PCIDevice;
struct PCIDevice *bridge = NULL;
struct PCIIFace *IPCI;
UBYTE my_bus, my_dev, my_fn;
U32 bars[6];
U32 cmd;
U32 cache_lat;
U32 irq_line;
U32 brctl;
U32 cnt;
U32 idx;

	if ( ! pd )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: SBR: no pcidev\n" );
		return( FALSE );
	}

	IPCI = usbbase->usb_IPCI;
	pd->GetAddress( & my_bus, & my_dev, & my_fn );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: SBR: device is at bus=%ld dev=%ld fn=%ld, scanning for parent bridge\n",
		(U32) my_bus, (U32) my_dev, (U32) my_fn );

	// Find PCI-to-PCI bridge (class 0x060400) whose secondary bus matches us.
	// Per autodoc, every FindDeviceTags result MUST be released with
	// FreeDevice after use. We FreeDevice non-matching candidates here and
	// FreeDevice the chosen bridge later, after the SBR pulse is complete.
	for ( idx = 0 ; idx < 32 ; idx++ )
	{
		struct PCIDevice *cand;
		U32 sec_bus;

		cand = IPCI->FindDeviceTags(
			FDT_Class,     0x00060400,
			FDT_ClassMask, 0x00FFFFFF,
			FDT_Index,     idx,
			TAG_END );

		if ( ! cand )
		{
			break;
		}

		sec_bus = cand->ReadConfigByte( PCI_SECONDARY_BUS );	// Secondary Bus Number

		usbbase->usb_IExec->DebugPrintF(
			"USB3: SBR: bridge[%ld] sec_bus=%ld\n", idx, sec_bus );

		if ( sec_bus == (U32) my_bus )
		{
			bridge = cand;
			break;
		}

		// Not our bridge -- release the interface immediately
		IPCI->FreeDevice( cand );
	}

	if ( ! bridge )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: SBR: no parent bridge found whose secondary bus matches %ld\n",
			(U32) my_bus );
		return( FALSE );
	}

	// Save our device's config-space state -- SBR will reset our BARs etc.
	for ( cnt = 0 ; cnt < 6 ; cnt++ )
	{
		bars[ cnt ] = pd->ReadConfigLong( PCI_BASE_ADDRESS_0 + ( cnt * 4 ) );
	}
	cmd       = pd->ReadConfigWord( PCI_COMMAND );
	cache_lat = pd->ReadConfigWord( PCI_CACHE_LINE_SIZE );		// CacheLineSize | LatencyTimer
	irq_line  = pd->ReadConfigByte( PCI_INTERRUPT_LINE );		// Interrupt Line

	usbbase->usb_IExec->DebugPrintF(
		"USB3: SBR: saved device state BAR0=0x%08lx CMD=0x%04lx IRQ=%ld\n",
		bars[0], cmd, (U32) irq_line );

	// Toggle SBR bit (bit 6) of bridge control register at offset 0x3E
	brctl = bridge->ReadConfigWord( PCI_BRIDGE_CONTROL );
	usbbase->usb_IExec->DebugPrintF(
		"USB3: SBR: bridge BridgeCtl=0x%04lx, asserting SBR (bit 6)\n", brctl );

	bridge->WriteConfigWord( PCI_BRIDGE_CONTROL, (UWORD)( brctl | ( 1U << 6 ) ) );

	// Read back to verify the SBR bit took
	{
		U32 chk = bridge->ReadConfigWord( PCI_BRIDGE_CONTROL );
		usbbase->usb_IExec->DebugPrintF(
			"USB3: SBR: BridgeCtl readback after assert=0x%04lx (bit 6 %s)\n",
			chk, ( chk & ( 1U << 6 ) ) ? "stuck" : "NOT SET - bridge ignored write!" );
	}

	// Probe device while held in reset -- if the link is actually down,
	// PCI config reads should return 0xFFFF/0xFFFFFFFF
	{
		U32 vid = pd->ReadConfigWord( PCI_VENDOR_ID );
		U32 bar0 = pd->ReadConfigLong( PCI_BASE_ADDRESS_0 );
		usbbase->usb_IExec->DebugPrintF(
			"USB3: SBR: while-held: device VendorID=0x%04lx BAR0=0x%08lx (0xFFFF/0xFFFFFFFF = link down, good)\n",
			vid, bar0 );
	}

	// Hold reset for >=2 ms (PCIe spec); we use 50 ms for safety
	HCD_WAIT_MS( hn, 50 );

	bridge->WriteConfigWord( PCI_BRIDGE_CONTROL, (UWORD)( brctl & ~( 1U << 6 ) ) );
	usbbase->usb_IExec->DebugPrintF(
		"USB3: SBR: SBR deasserted, waiting for link re-train ...\n" );

	// Wait for PCIe link to come back up; spec says >=100 ms
	HCD_WAIT_MS( hn, 200 );

	// Probe device after reset, before we restore -- if BAR0 is 0 the
	// link reset DID happen and the device is back in power-on state
	{
		U32 vid = pd->ReadConfigWord( PCI_VENDOR_ID );
		U32 bar0 = pd->ReadConfigLong( PCI_BASE_ADDRESS_0 );
		U32 cmd_post = pd->ReadConfigWord( PCI_COMMAND );
		usbbase->usb_IExec->DebugPrintF(
			"USB3: SBR: post-reset bare device: VendorID=0x%04lx BAR0=0x%08lx CMD=0x%04lx\n",
			vid, bar0, cmd_post );
	}

	// Restore our device's config-space state
	for ( cnt = 0 ; cnt < 6 ; cnt++ )
	{
		pd->WriteConfigLong( PCI_BASE_ADDRESS_0 + ( cnt * 4 ), bars[ cnt ] );
	}
	pd->WriteConfigWord( PCI_CACHE_LINE_SIZE, (UWORD)cache_lat );
	pd->WriteConfigByte( PCI_INTERRUPT_LINE, (UBYTE)irq_line );
	pd->WriteConfigWord( PCI_COMMAND,
		(UWORD)( cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER ) );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: SBR: device config restored, BAR0=0x%08lx CMD=0x%04lx\n",
		pd->ReadConfigLong( PCI_BASE_ADDRESS_0 ), pd->ReadConfigWord( PCI_COMMAND ) );

	// Release the bridge interface -- the SBR pulse is complete and we're
	// done using it. Matches the FindDeviceTags that found it.
	IPCI->FreeDevice( bridge );

	return( TRUE );
}

// Tier 6: PCIe Link Disable/Enable on the parent bridge. Different
// mechanism from SBR -- Link Disable (LNKCTL bit 4 in the bridge's PCIe
// Capability) drops the PCIe link to the Disabled LTSSM state. The
// downstream device sees electrical idle and per spec must reset its
// LTSSM; when we clear LD, both ends retrain. In theory this is enforced
// by the bridge's PHY, but the tested result on the X5000 matches SBR:
// the P5020 PEX accepts the bit (readback shows it set) yet the link
// never actually drops -- the device stays visible throughout.

static S32 _pci_link_disable_enable( struct USB2_HCDNode *hn )
{
struct USBBase *usbbase = hn->hn_USBBase;
struct PCIDevice *pd = hn->hn_PCIDevice;
struct PCIDevice *bridge = NULL;
struct PCIIFace *IPCI;
struct PCICapability *brexp;
UBYTE my_bus, my_dev, my_fn;
U32 bars[6];
U32 cmd;
U32 cache_lat;
U32 irq_line;
U32 lnkctl_off;
U32 lnkctl;
U32 cnt;
U32 idx;

	if ( ! pd )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: LinkDisable: no pcidev\n" );
		return( FALSE );
	}

	IPCI = usbbase->usb_IPCI;
	pd->GetAddress( & my_bus, & my_dev, & my_fn );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: LinkDisable: device at bus=%ld, scanning for parent bridge\n",
		(U32) my_bus );

	// Find PCI-to-PCI bridge whose secondary bus matches us (same as SBR).
	for ( idx = 0 ; idx < 32 ; idx++ )
	{
		struct PCIDevice *cand;
		U32 sec_bus;

		cand = IPCI->FindDeviceTags(
			FDT_Class,     0x00060400,
			FDT_ClassMask, 0x00FFFFFF,
			FDT_Index,     idx,
			TAG_END );

		if ( ! cand ) break;

		sec_bus = cand->ReadConfigByte( PCI_SECONDARY_BUS );
		if ( sec_bus == (U32) my_bus )
		{
			bridge = cand;
			break;
		}
		IPCI->FreeDevice( cand );
	}

	if ( ! bridge )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: LinkDisable: no parent bridge\n" );
		return( FALSE );
	}

	// Find PCIe Capability on the bridge.
	brexp = bridge->FindCapability( PCI_CAPABILITYID_EXP );
	if ( ! brexp )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: LinkDisable: bridge has no PCIe capability\n" );
		IPCI->FreeDevice( bridge );
		return( FALSE );
	}

	// LNKCTL is at PCIe-cap + 0x10 (16-bit register)
	lnkctl_off = brexp->CapOffset + 0x10;
	lnkctl = bridge->ReadConfigWord( lnkctl_off );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: LinkDisable: bridge PCIe cap at 0x%lx, LNKCTL offset=0x%lx, current=0x%04lx\n",
		brexp->CapOffset, lnkctl_off, lnkctl );

	// Save device's PCI config-space state -- LD will tear the link, the
	// chip's BARs/COMMAND will likely be cleared on link re-up.
	for ( cnt = 0 ; cnt < 6 ; cnt++ )
	{
		bars[ cnt ] = pd->ReadConfigLong( PCI_BASE_ADDRESS_0 + ( cnt * 4 ) );
	}
	cmd       = pd->ReadConfigWord( PCI_COMMAND );
	cache_lat = pd->ReadConfigWord( PCI_CACHE_LINE_SIZE );
	irq_line  = pd->ReadConfigByte( PCI_INTERRUPT_LINE );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: LinkDisable: saved BAR0=0x%08lx CMD=0x%04lx, asserting LD (bit 4) ...\n",
		bars[0], cmd );

	// Set LD (bit 4) -- bridge takes downstream link to Disabled
	bridge->WriteConfigWord( lnkctl_off, (UWORD)( lnkctl | ( 1U << 4 ) ) );

	// Verify the write took
	{
		U32 chk = bridge->ReadConfigWord( lnkctl_off );
		usbbase->usb_IExec->DebugPrintF(
			"USB3: LinkDisable: LNKCTL readback after assert=0x%04lx (LD bit %s)\n",
			chk, ( chk & ( 1U << 4 ) ) ? "stuck" : "NOT SET - bridge ignored write!" );
	}

	// Probe device while link is held down -- expect VendorID=0xFFFF
	{
		U32 vid = pd->ReadConfigWord( PCI_VENDOR_ID );
		usbbase->usb_IExec->DebugPrintF(
			"USB3: LinkDisable: while-disabled: device VendorID=0x%04lx (0xFFFF = link really dropped)\n",
			vid );
	}

	HCD_WAIT_MS( hn, 100 );

	// Clear LD -- link re-trains
	bridge->WriteConfigWord( lnkctl_off, (UWORD)( lnkctl & ~( 1U << 4 ) ) );
	usbbase->usb_IExec->DebugPrintF( "USB3: LinkDisable: LD cleared, waiting 300 ms for link re-train ...\n" );
	HCD_WAIT_MS( hn, 300 );

	// Probe device after link comes back
	{
		U32 vid = pd->ReadConfigWord( PCI_VENDOR_ID );
		U32 bar0 = pd->ReadConfigLong( PCI_BASE_ADDRESS_0 );
		usbbase->usb_IExec->DebugPrintF(
			"USB3: LinkDisable: post-restore bare device: VendorID=0x%04lx BAR0=0x%08lx\n",
			vid, bar0 );
	}

	// Restore PCI config-space state
	for ( cnt = 0 ; cnt < 6 ; cnt++ )
	{
		pd->WriteConfigLong( PCI_BASE_ADDRESS_0 + ( cnt * 4 ), bars[ cnt ] );
	}
	pd->WriteConfigWord( PCI_CACHE_LINE_SIZE, (UWORD)cache_lat );
	pd->WriteConfigByte( PCI_INTERRUPT_LINE, (UBYTE)irq_line );
	pd->WriteConfigWord( PCI_COMMAND,
		(UWORD)( cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER ) );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: LinkDisable: device config restored, BAR0=0x%08lx CMD=0x%04lx\n",
		pd->ReadConfigLong( PCI_BASE_ADDRESS_0 ), pd->ReadConfigWord( PCI_COMMAND ) );

	IPCI->FreeDevice( bridge );
	return( TRUE );
}

// Renesas XHCI_ZERO_64B_REGS quirk: explicitly zero the 64-bit MMIO
// registers (CRCR, DCBAAP, ERSTBA) after HCRST. Renesas chips retain
// the upper 32 bits across reset which can poison subsequent setup if
// init code only writes the low half.

static void _zero_64b_regs( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci = & hn->hn_HCD.XHCI;
struct USBBase *usbbase = hn->hn_USBBase;

	PCI_WRITELONG( xhci->OpBase + XHCI_CRCR_LO,    0 );
	PCI_WRITELONG( xhci->OpBase + XHCI_CRCR_HI,    0 );
	PCI_WRITELONG( xhci->OpBase + XHCI_DCBAAP_LO,  0 );
	PCI_WRITELONG( xhci->OpBase + XHCI_DCBAAP_HI,  0 );
	PCI_WRITELONG( xhci->RunBase + XHCI_ERSTBA_LO( 0 ), 0 );
	PCI_WRITELONG( xhci->RunBase + XHCI_ERSTBA_HI( 0 ), 0 );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Reset: zeroed 64b regs (Renesas XHCI_ZERO_64B_REGS quirk)\n" );
}

// Tier 2: PCI bus master/memory toggle -- lighter-weight than D3 cycle.

static void _pci_bus_master_toggle( struct USB2_HCDNode *hn )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
struct USBBase *usbbase = hn->hn_USBBase;
U32 cmd;

	if ( ! pd )
	{
		return;
	}

	cmd = pd->ReadConfigWord( PCI_COMMAND );
	usbbase->usb_IExec->DebugPrintF( "USB3: PCI cmd toggle: pre=0x%04lx\n", cmd );

	// Disable Bus Master + Memory
	pd->WriteConfigWord( PCI_COMMAND, cmd & ~( PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY ) );
	HCD_WAIT_MS( hn, 5 );

	// Re-enable
	pd->WriteConfigWord( PCI_COMMAND, cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER );
	HCD_WAIT_MS( hn, 5 );

	usbbase->usb_IExec->DebugPrintF( "USB3: PCI cmd toggle: done, post=0x%04lx\n",
		pd->ReadConfigWord( PCI_COMMAND ) );
}

// --

SEC_CODE S32 XHCI_Chip_Reset( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
U32 cmd;
U32 sts;
U32 cnt;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Reset" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// -- Initial state snapshot

	cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
	sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Reset: pre  USBCMD=0x%08lx USBSTS=0x%08lx\n", cmd, sts );

	// -- Fast-bail when chip is in HCE (Host Controller Error).
	//    Once USBSTS bit 12 is latched, only a hardware reset clears it.
	//    All six software recovery tiers below have been tested and proven
	//    non-functional against this state on the Renesas uPD720202 +
	//    P5020 PEX combo, so don't grind through them.

	if ( sts & XHCI_STS_HCE )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: ABORT - controller in unrecoverable error state\n"
			"USB3: Reset: ABORT - USBSTS.HCE=1, software cannot clear this\n"
			"USB3: Reset: ABORT - please MAINS POWER-CYCLE the system to recover\n" );
		USBERROR( "USB3: HCE latched - cold-power-cycle required" );
		goto bailout;
	}

	// -- CNR pre-state handling. CNR can be set in two situations:
	//    1. Cold-boot self-test still in progress (clears in <1 s, normal)
	//    2. Chip wedged from prior session (never clears, software-unreachable)
	//    We allow 2000 ms grace -- well past the longest documented cold-boot
	//    self-test (~1 s on Renesas) but short enough to bail fast on a wedge
	//    rather than grinding through 1.5 s of dud reset tiers.

	if ( sts & XHCI_STS_CNR )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: pre-state CNR=1 (chip still in self-test or wedged), waiting up to 2000 ms ...\n" );

		for ( cnt = 0 ; cnt < 2000 ; cnt++ )
		{
			HCD_WAIT_MS( hn, 1 );
			sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );
			if ( ! ( sts & XHCI_STS_CNR ) ) break;
		}

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: CNR-grace %ld ms, USBSTS=0x%08lx\n", cnt, sts );

		if ( sts & XHCI_STS_CNR )
		{
			usbbase->usb_IExec->DebugPrintF(
				"USB3: Reset: CNR still set after 2 s -- attempting HCRST anyway as a force-recovery.\n"
				"USB3: Reset: (xHCI spec says don't write op-regs while CNR=1, but Linux/Renesas\n"
				"USB3: Reset:  practice is to try HCRST -- it often forces CNR to clear)\n" );
			// Fall through to the normal halt+HCRST sequence below. The
			// 6-tier ladder will run; if HCRST takes effect, chip recovers.
			// If not, all six tiers will fail and we bail with the usual
			// "MAINS POWER-CYCLE" message.
		}
		else
		{
			usbbase->usb_IExec->DebugPrintF(
				"USB3: Reset: CNR cleared naturally - chip is ready, proceeding with HCRST\n" );
		}
	}

	// -- Halt the controller if running (xHCI spec: write 0 to RS, wait for HCH)

	if ( cmd & XHCI_CMD_RS )
	{
		cmd &= ~XHCI_CMD_RS;
		PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, cmd );

		for ( cnt = 0 ; cnt < 200 ; cnt++ )
		{
			HCD_WAIT_MS( hn, 1 );
			sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );
			if ( sts & XHCI_STS_HCH ) break;
		}

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: halt-wait %ld ms, USBSTS=0x%08lx\n", cnt, sts );

		if ( ! ( sts & XHCI_STS_HCH ) )
		{
			USBERROR( "USB3: Halt timeout" );
			goto bailout;
		}
	}
	else
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: Reset: chip already halted (RS=0)\n" );
	}

	// -- Force USBCMD to a known clean state before HCRST

	PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, 0 );
	HCD_WAIT_MS( hn, 1 );

	usbbase->usb_IExec->DebugPrintF( "USB3: Reset: controller halted, issuing HCRST\n" );

	// -- Tier 1: standard HCRST

	PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, XHCI_CMD_HCRST );
	cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );

	usbbase->usb_IExec->DebugPrintF(
		"USB3: Reset: HCRST issued, USBCMD readback=0x%08lx\n", cmd );

	// HCRST normally clears in microseconds. 200 ms is generous; longer
	// waits on this hardware never recover (proven by 5000 ms tests).
	cnt = _hcrst_wait( hn, 200 );

	cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
	usbbase->usb_IExec->DebugPrintF(
		"USB3: Reset: tier1 HCRST-wait %ld ms, USBCMD=0x%08lx\n", cnt, cmd );

	if ( cmd & XHCI_CMD_HCRST )
	{
		// -- Tier 2: PCI bus master / memory toggle

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: HCRST stuck, trying PCI bus master toggle ...\n" );

		_pci_bus_master_toggle( hn );

		// Re-issue HCRST
		PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, 0 );
		HCD_WAIT_MS( hn, 5 );
		PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, XHCI_CMD_HCRST );

		cnt = _hcrst_wait( hn, 200 );
		cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: tier2 HCRST-wait %ld ms, USBCMD=0x%08lx\n", cnt, cmd );
	}

	if ( cmd & XHCI_CMD_HCRST )
	{
		// -- Tier 3: PCI D3hot -> D0 power-state cycle

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: HCRST still stuck, trying D3->D0 power cycle ...\n" );

		_pci_power_cycle( hn );

		// Re-issue HCRST
		PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, 0 );
		HCD_WAIT_MS( hn, 10 );
		PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, XHCI_CMD_HCRST );

		cnt = _hcrst_wait( hn, 200 );
		cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: tier3 HCRST-wait %ld ms, USBCMD=0x%08lx\n", cnt, cmd );
	}

	if ( cmd & XHCI_CMD_HCRST )
	{
		// -- Tier 4: PCIe Function-Level Reset (FLR)
		// Processed by the PCIe core, not the xHCI logic, so it can work
		// even when HCRST is wedged -- if the device supports FLR at all
		// (this uPD720202 card does not; _pci_flr checks and bails).

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: HCRST still stuck, trying PCIe FLR ...\n" );

		if ( _pci_flr( hn ) )
		{
			// FLR returns the device to power-on state. USBCMD should
			// now read 0x00000000 with no further action needed.
			cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
			sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );
			usbbase->usb_IExec->DebugPrintF(
				"USB3: Reset: post-FLR USBCMD=0x%08lx USBSTS=0x%08lx\n", cmd, sts );

			if ( cmd & XHCI_CMD_HCRST )
			{
				// Some chips need an explicit HCRST after FLR to confirm
				PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, XHCI_CMD_HCRST );
				cnt = _hcrst_wait( hn, 200 );
				cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
				usbbase->usb_IExec->DebugPrintF(
					"USB3: Reset: tier4 post-FLR HCRST-wait %ld ms, USBCMD=0x%08lx\n",
					cnt, cmd );
			}
		}
	}

	if ( cmd & XHCI_CMD_HCRST )
	{
		// -- Tier 5: PCIe Secondary Bus Reset on the upstream bridge.
		// SBR is a bridge feature; the device itself cannot decline it
		// (though the P5020 PEX has been observed to no-op it -- see
		// _pci_sbr).

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: HCRST/FLR both failed, trying PCIe Secondary Bus Reset ...\n" );

		if ( _pci_sbr( hn ) )
		{
			cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
			sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );
			usbbase->usb_IExec->DebugPrintF(
				"USB3: Reset: post-SBR USBCMD=0x%08lx USBSTS=0x%08lx\n", cmd, sts );

			if ( cmd & XHCI_CMD_HCRST )
			{
				// SBR should have cleared HCRST; if still set, issue one more
				PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, XHCI_CMD_HCRST );
				cnt = _hcrst_wait( hn, 200 );
				cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
				usbbase->usb_IExec->DebugPrintF(
					"USB3: Reset: tier5 post-SBR HCRST-wait %ld ms, USBCMD=0x%08lx\n",
					cnt, cmd );
			}
		}
	}

	if ( cmd & XHCI_CMD_HCRST )
	{
		// -- Tier 6: PCIe Link Disable/Enable on the parent bridge.
		// Different mechanism from SBR -- LD tears down the PCIe link at
		// the bridge's PHY (in theory; see _pci_link_disable_enable).

		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: SBR/FLR all failed, trying PCIe Link Disable/Enable ...\n" );

		if ( _pci_link_disable_enable( hn ) )
		{
			cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
			sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );
			usbbase->usb_IExec->DebugPrintF(
				"USB3: Reset: post-LD USBCMD=0x%08lx USBSTS=0x%08lx\n", cmd, sts );

			if ( cmd & XHCI_CMD_HCRST )
			{
				PCI_WRITELONG( xhci->OpBase + XHCI_USBCMD, XHCI_CMD_HCRST );
				cnt = _hcrst_wait( hn, 200 );
				cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
				usbbase->usb_IExec->DebugPrintF(
					"USB3: Reset: tier6 post-LD HCRST-wait %ld ms, USBCMD=0x%08lx\n",
					cnt, cmd );
			}
		}
	}

	if ( cmd & XHCI_CMD_HCRST )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: Reset: ABORT - all 6 software recovery tiers exhausted\n"
			"USB3: Reset: ABORT - this hardware combination requires a MAINS POWER-CYCLE\n" );
		USBERROR( "USB3: Reset timeout (HCRST stuck after all recovery tiers)" );
		goto bailout;
	}

	// -- Wait for Controller Not Ready to clear

	for ( cnt = 0 ; cnt < 5000 ; cnt++ )
	{
		HCD_WAIT_MS( hn, 1 );
		sts = PCI_READLONG( xhci->OpBase + XHCI_USBSTS );
		if ( ! ( sts & XHCI_STS_CNR ) ) break;
	}

	cmd = PCI_READLONG( xhci->OpBase + XHCI_USBCMD );
	usbbase->usb_IExec->DebugPrintF(
		"USB3: Reset: CNR-wait %ld ms, USBCMD=0x%08lx USBSTS=0x%08lx\n",
		cnt, cmd, sts );

	if ( sts & XHCI_STS_CNR )
	{
		USBERROR( "USB3: CNR timeout" );
		goto bailout;
	}

	// -- Renesas XHCI_ZERO_64B_REGS quirk: zero 64-bit regs after reset
	_zero_64b_regs( hn );

	usbbase->usb_IExec->DebugPrintF( "USB3: Controller reset complete\n" );

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
