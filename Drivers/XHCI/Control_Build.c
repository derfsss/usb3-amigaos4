
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

// XHCI handles SET_ADDRESS internally via the Address Device command.

static S32 XHCI_Handle_SetAddress( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct _XHCI *xhci;
struct RealFunctionNode *fn;
U32 slotid;
U32 port;
U32 speed;
U32 desired_addr;

	struct USBBase *usbbase = hn->hn_USBBase;

	xhci = & hn->hn_HCD.XHCI;
	fn   = ioreq->req_Function;

	port  = fn->fkt_PortNr;
	speed = fn->fkt_Speed;

	// fn->fkt_Speed is what the HUB driver thinks the device is, which for
	// root-hub ports is unreliable (observed: always Full-Speed). Use the
	// negotiated speed that Port_Set_Reset cached from the post-reset
	// PORTSC frame instead -- that is the authoritative value for filling
	// in the Slot Context.
	if ( port < ( sizeof( xhci->PortSpeed ) / sizeof( xhci->PortSpeed[0] ) )
	&&   xhci->PortSpeed[ port ] != 0 )
	{
		U32 cached = xhci->PortSpeed[ port ];

		usbbase->usb_IExec->DebugPrintF(
			"USB3: SET_ADDRESS: using cached post-reset usb_speed=%ld for port %ld (HUB driver said %ld)\n",
			cached, port, speed );

		speed = cached;
	}
	else
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: SET_ADDRESS: no cached speed for port %ld, falling back to HUB driver value %ld\n",
			port, speed );
	}

	// Get desired address from setup packet (fn->fkt_Address is temporarily 0)
	desired_addr = LE_SWAP16( ioreq->req_Public.io_SetupData->Value );

	usbbase->usb_IExec->DebugPrintF( "XHCI: SET_ADDRESS intercepted (port=%ld speed=%ld addr=%ld)\n",
		port, speed, desired_addr );

	// Port_Set_Reset defers slot creation to this interception, so address
	// 0 normally has no slot yet. (A non-zero entry would come from an HCD
	// variant that pre-allocates the slot during port reset.)
	slotid = xhci->SlotID_ByAddress[0];

	usbbase->usb_IExec->DebugPrintF(
		"USB3: SET_ADDRESS: SlotID_ByAddress[0]=%ld\n", slotid );

	if ( ! slotid )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: SET_ADDRESS: no slot for address 0 yet, doing EnableSlot+SlotAlloc\n" );

		slotid = XHCI_Cmd_EnableSlot( hn );

		if ( ! slotid )
		{
			usbbase->usb_IExec->DebugPrintF(
				"USB3: SET_ADDRESS: EnableSlot FAILED\n" );
			USBERROR( "XHCI: SET_ADDRESS: Enable Slot failed" );
			ioreq->req_Public.io_Error = USB3Err_Host_HostError;
			return( TRUE );
		}

		// XHCI_Slot_Alloc expects a 0-based port index (it does +1 internally
		// to set the 1-based RHPORT field). fn->fkt_PortNr is 1-based.
		if ( ! XHCI_Slot_Alloc( hn, slotid, port - 1, speed ) )
		{
			usbbase->usb_IExec->DebugPrintF(
				"USB3: SET_ADDRESS: Slot_Alloc FAILED, releasing slot %ld\n", slotid );
			USBERROR( "XHCI: SET_ADDRESS: Slot Alloc failed" );
			XHCI_Cmd_DisableSlot( hn, slotid );
			ioreq->req_Public.io_Error = USB3Err_Stack_NoMemory;
			return( TRUE );
		}
	}

	// Issue Address Device command (bsr=0 for full address assignment)
	usbbase->usb_IExec->DebugPrintF(
		"USB3: SET_ADDRESS: calling AddressDevice(slot=%ld, bsr=0) -> assign addr %ld\n",
		slotid, desired_addr );

	if ( ! XHCI_Cmd_AddressDevice( hn, slotid, 0 ) )
	{
		usbbase->usb_IExec->DebugPrintF(
			"USB3: SET_ADDRESS: AddressDevice(BSR=0) FAILED - tearing down slot %ld\n", slotid );
		USBERROR( "XHCI: SET_ADDRESS: Address Device failed" );
		XHCI_Slot_Free( hn, slotid );
		XHCI_Cmd_DisableSlot( hn, slotid );
		ioreq->req_Public.io_Error = USB3Err_Host_HostError;
		xhci->SlotID_ByAddress[0] = 0;	// invalidate stale mapping
		return( TRUE );
	}

	// Clear address 0 mapping
	xhci->SlotID_ByAddress[0] = 0;

	// Store address-to-slot mapping for future transfers
	if ( desired_addr < 128 )
	{
		xhci->SlotID_ByAddress[ desired_addr ] = (U8) slotid;
	}

	ioreq->req_Public.io_Error = 0;
	ioreq->req_Public.io_Actual = 0;

	usbbase->usb_IExec->DebugPrintF( "XHCI: SET_ADDRESS complete (slot=%ld addr=%ld)\n",
		slotid, desired_addr );

	// Return TRUE -- SET_ADDRESS completed synchronously
	return( TRUE );
}

// --

SEC_CODE S32 XHCI_Control_Build( struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct RealFunctionNode *fn;
struct USB3_SetupData *setup;
struct _XHCI *xhci;
struct XHCI_Slot *slot;
struct XHCI_Ring *ring;
struct XHCI_TRB trb;
S32 handled;
U32 slotid;
U32 data_len;
U32 trt;
U32 status_dir;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "XHCI : XHCI_Control_Build" );

	fn    = ioreq->req_Function;
	setup = ioreq->req_Public.io_SetupData;
	xhci  = & hn->hn_HCD.XHCI;

	handled = TRUE;

	if ( ! setup )
	{
		USBERROR( "XHCI: Control_Build: no setup data" );
		ioreq->req_Public.io_Error = USB3Err_Stack_InvalidStructure;
		goto bailout;
	}

	// -- SET_ADDRESS is handled internally by XHCI

	if (( setup->RequestType == ( REQTYPE_Write | REQTYPE_Standard | REQTYPE_Device ) )
	&&	( setup->RequestCode == REQCODE_Set_Address ))
	{
		handled = XHCI_Handle_SetAddress( hn, ioreq );
		goto bailout;
	}

	// -- SET_CONFIGURATION: set up endpoint contexts before sending to device

	if (( setup->RequestType == ( REQTYPE_Write | REQTYPE_Standard | REQTYPE_Device ) )
	&&	( setup->RequestCode == REQCODE_Set_Configuration ))
	{
		U32 cfg_slotid = xhci->SlotID_ByAddress[ fn->fkt_Address ];

		// Use fkt_Config_Active if set, else use first config in list
		struct USB3_ConfigNode *cfg_to_use = fn->fkt_Config_Active;

		if ( ! cfg_to_use )
		{
			cfg_to_use = fn->fkt_Configs.uh_Head;
		}

		usbbase->usb_IExec->DebugPrintF( "XHCI: SET_CONFIG: slot=%ld cfgActive=%p cfgHead=%p cfgToUse=%p\n",
			cfg_slotid, fn->fkt_Config_Active, fn->fkt_Configs.uh_Head, cfg_to_use );

		if ( cfg_slotid && cfg_to_use )
		{
			// Temporarily set active config for ConfigureEndpoints to use
			struct USB3_ConfigNode *saved = fn->fkt_Config_Active;
			fn->fkt_Config_Active = cfg_to_use;

			XHCI_Slot_ConfigureEndpoints( hn, cfg_slotid, fn );

			fn->fkt_Config_Active = saved;
		}
	}

	// -- General control transfer: Build Setup/Data/Status TRBs

	// Look up slot ID from device address
	slotid = xhci->SlotID_ByAddress[ fn->fkt_Address ];

	if ( ! slotid )
	{
		USBERROR( "XHCI: Control_Build: no slot for address %ld", (U32) fn->fkt_Address );
		ioreq->req_Public.io_Error = USB3Err_Stack_FunctionNotFound;
		goto bailout;
	}

	slot = xhci->Slots[ slotid ];

	if ( ! slot )
	{
		USBERROR( "XHCI: Control_Build: slot %ld not allocated", slotid );
		ioreq->req_Public.io_Error = USB3Err_Stack_FunctionNotFound;
		goto bailout;
	}

	ring = & slot->transfer_ring[0];	// EP0

	// -- Initialize IORequest XHCI state

	MEM_SET( & ioreq->req_HCD.XHCI, 0, sizeof( struct __xhci ) );
	ioreq->req_HCD.XHCI.SlotID = slotid;
	ioreq->req_HCD.XHCI.DCI    = 1;		// EP0

	// -- Determine transfer type

	data_len = ioreq->req_Public.io_Length;

	if ( data_len == 0 )
	{
		trt = XHCI_TRB_TRT_NODATA;
		status_dir = XHCI_TRB_DIR_IN;		// No data -> status IN
	}
	else if ( ioreq->req_Public.io_Command == CMD_READ )
	{
		trt = XHCI_TRB_TRT_IN;
		status_dir = 0;						// Data IN -> status OUT
	}
	else
	{
		trt = XHCI_TRB_TRT_OUT;
		status_dir = XHCI_TRB_DIR_IN;		// Data OUT -> status IN
	}

	// -- Setup Stage TRB (type=2, IDT=Immediate Data)

	usbbase->usb_IExec->DebugPrintF( "XHCI: Control_Build: Setup TRB (req=0x%02lx len=%ld)\n",
		(U32) setup->RequestCode, data_len );

	{
		U32 *sd32 = (U32 *) setup;

		MEM_SET( & trb, 0, sizeof( trb ) );

		// Setup packet goes inline as immediate data (already in LE wire format)
		trb.trb_param_lo = sd32[0];
		trb.trb_param_hi = sd32[1];
		trb.trb_status   = LE_SWAP32( XHCI_TRB_SET_XFERLEN( 8 ) | XHCI_TRB_SET_INTR( 0 ) );
		trb.trb_control  = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_SETUP ) | XHCI_TRB_IDT | trt );

		XHCI_Ring_Enqueue_TRB( hn, ring, & trb );
	}

	// -- Data Stage TRB (type=3, optional)

	if ( data_len > 0 )
	{
		PTR  bounce;
		U32  bounce_phy;

		// Allocate DMA bounce buffer
		bounce = MEMORY_ALLOC( MEMID_HCD_20k, FALSE );

		if ( ! bounce )
		{
			USBERROR( "XHCI: Control_Build: error allocating DMA buffer" );
			ioreq->req_Public.io_Error = USB3Err_Stack_NoMemory;
			goto bailout;
		}

		bounce_phy = ((struct Mem_FreeNode *) bounce)->mfn_Addr;

		// Store in IORequest for later copy-back and free
		ioreq->req_HCD.XHCI.DataBuffer     = bounce;
		ioreq->req_HCD.XHCI.DataBufferPhy  = bounce_phy;
		ioreq->req_HCD.XHCI.DataBufferLen  = data_len;
		ioreq->req_HCD.XHCI.DataBufferPool = MEMID_HCD_20k;

		// For OUT transfers, copy data to bounce buffer
		if (( ioreq->req_Public.io_Command != CMD_READ ) && ( ioreq->req_Public.io_Data ))
		{
			MEM_COPY( ioreq->req_Public.io_Data, bounce, data_len );
		}
		else
		{
			MEM_SET( bounce, 0, data_len );
		}

		MEM_SET( & trb, 0, sizeof( trb ) );

		trb.trb_param_lo = LE_SWAP32( bounce_phy );
		trb.trb_param_hi = 0;
		trb.trb_status   = LE_SWAP32( XHCI_TRB_SET_XFERLEN( data_len ) | XHCI_TRB_SET_TDSIZE( 0 ) | XHCI_TRB_SET_INTR( 0 ) );
		trb.trb_control  = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_DATA ) |
			( ( ioreq->req_Public.io_Command == CMD_READ ) ? XHCI_TRB_DIR_IN : 0 ) );

		XHCI_Ring_Enqueue_TRB( hn, ring, & trb );
	}

	// -- Status Stage TRB (type=4, IOC=1)

	MEM_SET( & trb, 0, sizeof( trb ) );

	trb.trb_status  = 0;
	trb.trb_control = LE_SWAP32( XHCI_TRB_SET_TYPE( XHCI_TRB_STATUS ) | XHCI_TRB_IOC | status_dir );

	XHCI_Ring_Enqueue_TRB( hn, ring, & trb );

	// -- Transfer needs hardware processing

	handled = FALSE;

bailout:

	if ( ! ioreq->req_Public.io_Error )
	{
		// Error might have been set by subroutines
	}

	TASK_NAME_LEAVE();

	return( handled );
}

// --
