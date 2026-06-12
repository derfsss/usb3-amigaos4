
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

SEC_CODE S32 XHCI_Chip_Preinit( struct USB2_HCDNode *hn )
{
struct _XHCI *xhci;
U32 hcs1;
U32 hcs2;
U32 hcc1;
U32 val;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Chip_Preinit" );

	retval = FALSE;
	xhci = & hn->hn_HCD.XHCI;

	// -- Card identification via PCI config space

	if ( hn->hn_PCIDevice )
	{
		struct PCIDevice *pd = hn->hn_PCIDevice;
		U32 vid = pd->ReadConfigWord( PCI_VENDOR_ID );
		U32 did = pd->ReadConfigWord( PCI_VENDOR_ID + 2 );
		U32 svid = pd->ReadConfigWord( PCI_SUBSYSTEM_VENDOR_ID );
		U32 sdid = pd->ReadConfigWord( PCI_SUBSYSTEM_ID );
		U32 rev  = pd->ReadConfigByte( PCI_REVISION_ID );

		usbbase->usb_IExec->DebugPrintF(
			"USB3: PCI ID: vendor=0x%04lx device=0x%04lx rev=0x%02lx subsys=0x%04lx:0x%04lx\n",
			vid, did, rev, svid, sdid );

		// -- Walk PCIe Extended Capability chain (starts at config offset 0x100)
		// Each ext-cap header dword: bits 0-15 = CapID, 16-19 = Version, 20-31 = NextPtr.
		// VSEC (CapID 0x000B) has a 2nd dword: bits 0-15 = VSEC ID, 16-19 = VSEC Rev,
		// 20-31 = VSEC Length in bytes (including the 8-byte standard header).
		//
		// Diagnostic dump only. Tested result on this uPD720202 card: a single
		// ext cap at 0x100 with the Renesas vendor CapID 0x1912 holding 16 bytes
		// of identification metadata -- no chip-specific reset register here.
		// (Linux's xhci-pci-renesas.c uses similar vendor blocks for firmware
		// download on the firmware-loadable uPD720201 variant.)
		{
			U32 ec_off = 0x100;
			U32 ec_idx;

			for ( ec_idx = 0 ; ec_idx < 16 ; ec_idx++ )
			{
				U32 ec_w0   = pd->ReadConfigLong( ec_off );
				U32 ec_id   = ec_w0 & 0xFFFFU;
				U32 ec_ver  = ( ec_w0 >> 16 ) & 0xFU;
				U32 ec_next = ( ec_w0 >> 20 ) & 0xFFFU;

				if ( ec_w0 == 0xFFFFFFFFUL || ec_w0 == 0 )
				{
					usbbase->usb_IExec->DebugPrintF(
						"USB3: PCIeExtCap[%ld] off=0x%lx EMPTY (w0=0x%08lx)\n",
						ec_idx, ec_off, ec_w0 );
					break;
				}

				usbbase->usb_IExec->DebugPrintF(
					"USB3: PCIeExtCap[%ld] off=0x%lx CapID=0x%04lx ver=%ld next=0x%lx w0=0x%08lx\n",
					ec_idx, ec_off, ec_id, ec_ver, ec_next, ec_w0 );

				// VSEC: decode header, then dump body dwords
				if ( ec_id == 0x000B )
				{
					U32 ec_w1 = pd->ReadConfigLong( ec_off + 4 );
					U32 vsec_id  = ec_w1 & 0xFFFFU;
					U32 vsec_rev = ( ec_w1 >> 16 ) & 0xFU;
					U32 vsec_len = ( ec_w1 >> 20 ) & 0xFFFU;
					U32 body_dws;
					U32 di;

					usbbase->usb_IExec->DebugPrintF(
						"USB3:   VSEC: ID=0x%04lx rev=%ld length=%ld bytes (%ld body dwords)\n",
						vsec_id, vsec_rev, vsec_len,
						vsec_len > 8 ? ( vsec_len - 8 ) / 4 : 0 );

					body_dws = vsec_len > 8 ? ( vsec_len - 8 ) / 4 : 0;
					if ( body_dws > 14 ) body_dws = 14;	// cap output

					for ( di = 0 ; di < body_dws ; di++ )
					{
						U32 dw = pd->ReadConfigLong( ec_off + 8 + ( di * 4 ) );
						usbbase->usb_IExec->DebugPrintF(
							"USB3:   VSEC body[%ld] off=0x%lx = 0x%08lx\n",
							di, ec_off + 8 + ( di * 4 ), dw );
					}
				}
				else
				{
					// Non-VSEC: dump first 4 dwords for context
					U32 di;
					for ( di = 1 ; di < 4 ; di++ )
					{
						U32 dw = pd->ReadConfigLong( ec_off + ( di * 4 ) );
						usbbase->usb_IExec->DebugPrintF(
							"USB3:   ExtCap body[%ld] off=0x%lx = 0x%08lx\n",
							di, ec_off + ( di * 4 ), dw );
					}
				}

				if ( ec_next == 0 || ec_next < 0x100 )
				{
					break;
				}
				ec_off = ec_next;
			}
		}
	}

	// Read Capability Register Length
	val = PCI_READBYTE( XHCI_CAPLENGTH );
	xhci->CapLength = val;

	usbbase->usb_IExec->DebugPrintF( "XHCI: CapLength = %ld\n", val );

	// Compute register base offsets
	xhci->OpBase       = xhci->CapLength;
	xhci->DoorbellBase = PCI_READLONG( XHCI_DBOFF ) & ~0x3U;
	xhci->RunBase      = PCI_READLONG( XHCI_RTSOFF ) & ~0x1FU;

	usbbase->usb_IExec->DebugPrintF( "XHCI: OpBase=0x%lx RunBase=0x%lx DoorbellBase=0x%lx\n",
		xhci->OpBase, xhci->RunBase, xhci->DoorbellBase );

	// Read Structural Parameters 1
	hcs1 = PCI_READLONG( XHCI_HCSPARAMS1 );

	xhci->MaxSlots = XHCI_HCS1_MAXSLOTS( hcs1 );
	xhci->MaxIntrs = XHCI_HCS1_MAXINTRS( hcs1 );
	xhci->MaxPorts = XHCI_HCS1_MAXPORTS( hcs1 );

	usbbase->usb_IExec->DebugPrintF( "XHCI: MaxSlots=%ld MaxIntrs=%ld MaxPorts=%ld\n",
		xhci->MaxSlots, xhci->MaxIntrs, xhci->MaxPorts );

	if ( xhci->MaxPorts == 0 )
	{
		USBERROR( "XHCI: MaxPorts is 0 -- aborting" );
		goto bailout;
	}

	// Read Structural Parameters 2 -- scratchpad count
	hcs2 = PCI_READLONG( XHCI_HCSPARAMS2 );
	xhci->ScratchpadCount = XHCI_HCS2_MAXSCRATCH( hcs2 );

	usbbase->usb_IExec->DebugPrintF( "XHCI: ScratchpadCount=%ld\n", xhci->ScratchpadCount );

	// Read Capability Parameters 1
	hcc1 = PCI_READLONG( XHCI_HCCPARAMS1 );

	// Context size: 32 bytes (CSZ=0) or 64 bytes (CSZ=1)
	xhci->ContextSize = ( hcc1 & XHCI_HCC1_CSZ ) ? 64 : 32;

	usbbase->usb_IExec->DebugPrintF( "XHCI: ContextSize=%ld AC64=%ld\n",
		xhci->ContextSize, ( hcc1 & XHCI_HCC1_AC64 ) ? 1 : 0 );

	// Read page size from operational registers
	val = PCI_READLONG( xhci->OpBase + XHCI_PAGESIZE );
	xhci->PageSize = ( val & 0xFFFF ) << 12;	// Bit N set means 2^(N+12) bytes

	if ( xhci->PageSize == 0 )
	{
		xhci->PageSize = 4096;
	}

	usbbase->usb_IExec->DebugPrintF( "XHCI: PageSize=%ld\n", xhci->PageSize );

	// -- Dump Supported Protocol Capabilities (xECP scan)
	// HCCPARAMS1[31:16] = xECP offset in dwords from cap base.
	// Each capability is a chain: word0 = (Next<<8)|CapID. Cap ID 2 = Supported Protocol.
	{
		U32 xecp_dw_off = ( hcc1 >> 16 ) & 0xFFFFU;

		usbbase->usb_IExec->DebugPrintF(
			"USB3: HCCPARAMS1=0x%08lx, xECP offset = %ld dwords (0x%lx bytes)\n",
			hcc1, xecp_dw_off, xecp_dw_off * 4 );

		if ( xecp_dw_off != 0 )
		{
			U32 cap_off = xecp_dw_off * 4;
			U32 guard;

			for ( guard = 0 ; guard < 32 ; guard++ )
			{
				U32 w0 = PCI_READLONG( cap_off );
				U32 cap_id = w0 & 0xFFU;
				U32 next   = ( w0 >> 8 ) & 0xFFU;

				usbbase->usb_IExec->DebugPrintF(
					"USB3: ExtCap[%ld] off=0x%lx CapID=%ld next=%ld word0=0x%08lx\n",
					guard, cap_off, cap_id, next, w0 );

				if ( cap_id == 2 )		// Supported Protocol Capability
				{
					U32 w1 = PCI_READLONG( cap_off + 4 );
					U32 w2 = PCI_READLONG( cap_off + 8 );
					U32 minor_rev = ( w0 >> 16 ) & 0xFFU;
					U32 major_rev = ( w0 >> 24 ) & 0xFFU;
					U32 port_off  = w2 & 0xFFU;
					U32 port_cnt  = ( w2 >> 8 ) & 0xFFU;
					U32 psic      = ( w2 >> 28 ) & 0xFU;
					char name[5];
					name[0] = (char)( ( w1 >>  0 ) & 0xFFU );
					name[1] = (char)( ( w1 >>  8 ) & 0xFFU );
					name[2] = (char)( ( w1 >> 16 ) & 0xFFU );
					name[3] = (char)( ( w1 >> 24 ) & 0xFFU );
					name[4] = 0;

					usbbase->usb_IExec->DebugPrintF(
						"USB3:   Supported Protocol '%s' v%ld.%02lx ports %ld..%ld (count=%ld) PSIC=%ld\n",
						name, major_rev, minor_rev,
						port_off, port_off + port_cnt - 1, port_cnt, psic );
				}

				if ( next == 0 )
				{
					break;
				}

				cap_off += next * 4;
			}
		}
	}

	// Set root hub port count for the stack
	hn->hn_HUB_NumPorts = xhci->MaxPorts;

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
