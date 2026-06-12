
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB2_ALL_H
#error Include "usb2_all.h" first
#endif

#ifndef INC_PRIVATE_HCD_XHCI_H
#define INC_PRIVATE_HCD_XHCI_H

/***************************************************************************/
// XHCI Capability Registers

#define XHCI_CAPLENGTH				0x00U		// RO : Capability Register Length (byte)
#define XHCI_HCIVERSION				0x02U		// RO : Interface Version (word)
#define XHCI_HCSPARAMS1				0x04U		// RO : Structural Parameters 1
#define XHCI_HCSPARAMS2				0x08U		// RO : Structural Parameters 2
#define XHCI_HCSPARAMS3				0x0CU		// RO : Structural Parameters 3
#define XHCI_HCCPARAMS1				0x10U		// RO : Capability Parameters 1
#define XHCI_DBOFF					0x14U		// RO : Doorbell Offset
#define XHCI_RTSOFF					0x18U		// RO : Runtime Register Space Offset

// HCSPARAMS1 fields
#define XHCI_HCS1_MAXSLOTS(x)		((x) & 0xFFU)
#define XHCI_HCS1_MAXINTRS(x)		(((x) >> 8) & 0x7FFU)
#define XHCI_HCS1_MAXPORTS(x)		(((x) >> 24) & 0xFFU)

// HCSPARAMS2 fields
#define XHCI_HCS2_MAXSCRATCHLO(x)	(((x) >> 27) & 0x1FU)
#define XHCI_HCS2_MAXSCRATCHHI(x)	(((x) >> 21) & 0x1FU)
#define XHCI_HCS2_MAXSCRATCH(x)	((XHCI_HCS2_MAXSCRATCHHI(x) << 5) | XHCI_HCS2_MAXSCRATCHLO(x))

// HCCPARAMS1 fields
#define XHCI_HCC1_AC64				0x00000001UL	// 64-bit Addressing Capability
#define XHCI_HCC1_CSZ				0x00000004UL	// Context Size (0=32 byte, 1=64 byte)

/***************************************************************************/
// XHCI Operational Registers (offset from OpBase)

#define XHCI_USBCMD					0x00U		// RW : USB Command
#define XHCI_USBSTS					0x04U		// RW : USB Status
#define XHCI_PAGESIZE				0x08U		// RO : Page Size
#define XHCI_DNCTRL					0x14U		// RW : Device Notification Control
#define XHCI_CRCR_LO				0x18U		// RW : Command Ring Control (low 32)
#define XHCI_CRCR_HI				0x1CU		// RW : Command Ring Control (high 32)
#define XHCI_DCBAAP_LO				0x30U		// RW : Device Context Base Address Array Pointer (low)
#define XHCI_DCBAAP_HI				0x34U		// RW : Device Context Base Address Array Pointer (high)
#define XHCI_CONFIG					0x38U		// RW : Configure
#define XHCI_PORTSC(n)				(0x400U + (0x10U * (n)))	// Port Status and Control (0-based)
#define XHCI_PORTPMSC(n)			(0x404U + (0x10U * (n)))	// Port Power Management
#define XHCI_PORTLI(n)				(0x408U + (0x10U * (n)))	// Port Link Info

// USBCMD bits
#define XHCI_CMD_RS					0x00000001UL	// Run/Stop
#define XHCI_CMD_HCRST				0x00000002UL	// Host Controller Reset
#define XHCI_CMD_INTE				0x00000004UL	// Interrupter Enable
#define XHCI_CMD_HSEE				0x00000008UL	// Host System Error Enable

// USBSTS bits
#define XHCI_STS_HCH				0x00000001UL	// HCHalted
#define XHCI_STS_HSE				0x00000004UL	// Host System Error
#define XHCI_STS_EINT				0x00000008UL	// Event Interrupt
#define XHCI_STS_PCD				0x00000010UL	// Port Change Detect
#define XHCI_STS_CNR				0x00000800UL	// Controller Not Ready
#define XHCI_STS_HCE				0x00001000UL	// Host Controller Error (unrecoverable)

// PORTSC bits
#define XHCI_PS_CCS				0x00000001UL	// Current Connect Status
#define XHCI_PS_PED				0x00000002UL	// Port Enabled/Disabled
#define XHCI_PS_OCA				0x00000008UL	// Over-current Active
#define XHCI_PS_PR					0x00000010UL	// Port Reset
#define XHCI_PS_PLS_MASK			0x000001E0UL	// Port Link State
#define XHCI_PS_PLS_SHIFT			5
#define XHCI_PS_PP					0x00000200UL	// Port Power
#define XHCI_PS_SPEED_MASK			0x00003C00UL	// Port Speed
#define XHCI_PS_SPEED_SHIFT			10
#define XHCI_PS_CSC				0x00020000UL	// Connect Status Change
#define XHCI_PS_PEC				0x00040000UL	// Port Enable Change
#define XHCI_PS_WRC				0x00080000UL	// Warm Port Reset Change
#define XHCI_PS_OCC				0x00100000UL	// Over-current Change
#define XHCI_PS_PRC				0x00200000UL	// Port Reset Change
#define XHCI_PS_PLC				0x00400000UL	// Port Link State Change
#define XHCI_PS_LWS				0x00010000UL	// Port Link State Write Strobe (commits PLS write)
#define XHCI_PS_WPR				0x80000000UL	// Warm Port Reset (RW1S)

// PORTSC RW1C change bits -- writing 1 clears the change flag
#define XHCI_PS_CHANGE_MASK			(XHCI_PS_CSC | XHCI_PS_PEC | XHCI_PS_WRC | XHCI_PS_OCC | XHCI_PS_PRC | XHCI_PS_PLC)

// PORTSC bits that trigger an ACTION when written as 1: PED is RW1C and
// writing 1 DISABLES the port, PR/WPR are RW1S and start a reset, LWS
// commits a PLS change. A read-modify-write of PORTSC that preserves any
// of these re-triggers the action -- e.g. writing back PED=1 while
// clearing PRC silently disables the port just enabled by the reset.
// Same idea as Linux's xhci_port_state_to_neutral().
#define XHCI_PS_W1_ACTION_MASK			(XHCI_PS_PED | XHCI_PS_PR | XHCI_PS_WPR | XHCI_PS_LWS)

// Everything that must be stripped from a PORTSC value before writing it back
#define XHCI_PS_WRITE_STRIP_MASK		(XHCI_PS_CHANGE_MASK | XHCI_PS_W1_ACTION_MASK)

// Port speed values (from PORTSC)
#define XHCI_PS_SPEED_FS			1		// Full Speed
#define XHCI_PS_SPEED_LS			2		// Low Speed
#define XHCI_PS_SPEED_HS			3		// High Speed
#define XHCI_PS_SPEED_SS			4		// SuperSpeed

// Port Link State values
#define XHCI_PLS_U0				0		// Enabled
#define XHCI_PLS_U3				3		// Suspended
#define XHCI_PLS_DISABLED			4		// Disabled
#define XHCI_PLS_RXDETECT			5		// Disconnected
#define XHCI_PLS_POLLING			7		// Polling
#define XHCI_PLS_RESUME				15		// Resume

// CRCR bits
#define XHCI_CRCR_RCS				0x00000001UL	// Ring Cycle State
#define XHCI_CRCR_CS				0x00000002UL	// Command Stop
#define XHCI_CRCR_CA				0x00000004UL	// Command Abort
#define XHCI_CRCR_CRR				0x00000008UL	// Command Ring Running

/***************************************************************************/
// XHCI Runtime Registers (offset from RunBase)

#define XHCI_IMAN(n)				(0x20U + (0x20U * (n)))		// Interrupter Management
#define XHCI_IMOD(n)				(0x24U + (0x20U * (n)))		// Interrupter Moderation
#define XHCI_ERSTSZ(n)				(0x28U + (0x20U * (n)))		// Event Ring Segment Table Size
#define XHCI_ERSTBA_LO(n)			(0x30U + (0x20U * (n)))		// ERST Base Address (low)
#define XHCI_ERSTBA_HI(n)			(0x34U + (0x20U * (n)))		// ERST Base Address (high)
#define XHCI_ERDP_LO(n)			(0x38U + (0x20U * (n)))		// Event Ring Dequeue Pointer (low)
#define XHCI_ERDP_HI(n)			(0x3CU + (0x20U * (n)))		// Event Ring Dequeue Pointer (high)

// IMAN bits
#define XHCI_IMAN_IP				0x00000001UL	// Interrupt Pending
#define XHCI_IMAN_IE				0x00000002UL	// Interrupt Enable

// ERDP bits
#define XHCI_ERDP_EHB				0x00000008UL	// Event Handler Busy

/***************************************************************************/
// XHCI Doorbell Register

#define XHCI_DB_HOST				0				// Doorbell 0 = Host Controller (command ring)
#define XHCI_DB_TARGET(ep)			(ep)			// Doorbell target = DCI (Device Context Index)

/***************************************************************************/
// TRB (Transfer Request Block) -- 16 bytes

struct XHCI_TRB
{
	U32		trb_param_lo;		// Parameter low (or data buffer pointer low)
	U32		trb_param_hi;		// Parameter high (or data buffer pointer high)
	U32		trb_status;			// Status / transfer length / interrupter target
	U32		trb_control;		// Control: TRB type, flags, cycle bit
};

#define XHCI_TRB_SIZE				16
#define XHCI_TRB_RING_SIZE			256				// Number of TRBs per ring (must be power of 2)

// TRB control field bits
#define XHCI_TRB_CYCLE				0x00000001UL	// Cycle bit
#define XHCI_TRB_ENT				0x00000002UL	// Evaluate Next TRB
#define XHCI_TRB_ISP				0x00000004UL	// Interrupt on Short Packet
#define XHCI_TRB_NS				0x00000008UL	// No Snoop
#define XHCI_TRB_CHAIN				0x00000010UL	// Chain bit
#define XHCI_TRB_IOC				0x00000020UL	// Interrupt On Completion
#define XHCI_TRB_IDT				0x00000040UL	// Immediate Data
#define XHCI_TRB_BSR				0x00000200UL	// Block Set Address Request (Address Device)
#define XHCI_TRB_TC				0x00000002UL	// Toggle Cycle (for Link TRB)
#define XHCI_TRB_DIR_IN			0x00010000UL	// Data direction IN (for Data Stage TRB)

// TRB type (bits 10-15 of control)
#define XHCI_TRB_TYPE_SHIFT			10
#define XHCI_TRB_SET_TYPE(x)		((x) << XHCI_TRB_TYPE_SHIFT)
#define XHCI_TRB_GET_TYPE(x)		(((x) >> XHCI_TRB_TYPE_SHIFT) & 0x3FU)

// TRB types -- Transfer
#define XHCI_TRB_NORMAL			1
#define XHCI_TRB_SETUP				2
#define XHCI_TRB_DATA				3
#define XHCI_TRB_STATUS			4
#define XHCI_TRB_ISOCH				5
#define XHCI_TRB_LINK				6
#define XHCI_TRB_EVENT_DATA		7
#define XHCI_TRB_NOOP				8

// TRB types -- Command
#define XHCI_TRB_ENABLE_SLOT		9
#define XHCI_TRB_DISABLE_SLOT		10
#define XHCI_TRB_ADDRESS_DEVICE		11
#define XHCI_TRB_CONFIGURE_EP		12
#define XHCI_TRB_EVALUATE_CTX		13
#define XHCI_TRB_RESET_EP			14
#define XHCI_TRB_STOP_EP			15
#define XHCI_TRB_SET_TR_DEQUEUE		16
#define XHCI_TRB_RESET_DEVICE		17
#define XHCI_TRB_NOOP_CMD			23

// TRB types -- Event
#define XHCI_TRB_TRANSFER_EVENT		32
#define XHCI_TRB_CMD_COMPLETION		33
#define XHCI_TRB_PORT_STATUS_CHG	34
#define XHCI_TRB_HOST_CTRL_ERROR	37

// TRB status field -- transfer length (bits 0-16)
#define XHCI_TRB_SET_XFERLEN(x)	((x) & 0x1FFFFU)
#define XHCI_TRB_GET_XFERLEN(x)	((x) & 0x1FFFFU)

// TRB status field -- completion code (bits 24-31, in event TRBs)
#define XHCI_TRB_GET_COMPCODE(x)	(((x) >> 24) & 0xFFU)

// TRB status field -- interrupter target (bits 22-31)
#define XHCI_TRB_SET_INTR(x)		(((x) & 0x3FFU) << 22)

// TRB status field -- TD Size (bits 17-21)
#define XHCI_TRB_SET_TDSIZE(x)		(((x) & 0x1FU) << 17)

// TRB control field -- Slot ID (bits 24-31, in command TRBs)
#define XHCI_TRB_SET_SLOT(x)		(((x) & 0xFFU) << 24)
#define XHCI_TRB_GET_SLOT(x)		(((x) >> 24) & 0xFFU)

// TRB control field -- Endpoint ID (bits 16-20, in command TRBs)
#define XHCI_TRB_SET_EP(x)			(((x) & 0x1FU) << 16)

// Setup TRB -- Transfer Type (bits 16-17 of control)
#define XHCI_TRB_TRT_NODATA		(0U << 16)
#define XHCI_TRB_TRT_OUT			(2U << 16)
#define XHCI_TRB_TRT_IN			(3U << 16)

// Completion codes
#define XHCI_CC_INVALID				0
#define XHCI_CC_SUCCESS				1
#define XHCI_CC_DATA_BUFFER			2
#define XHCI_CC_BABBLE				3
#define XHCI_CC_USB_TRANSACTION		4
#define XHCI_CC_TRB_ERROR			5
#define XHCI_CC_STALL				6
#define XHCI_CC_SHORT_PACKET		13
#define XHCI_CC_EVENT_RING_FULL		21
#define XHCI_CC_COMMAND_RING_STOPPED	24
#define XHCI_CC_COMMAND_ABORTED		25
#define XHCI_CC_STOPPED				26
#define XHCI_CC_STOPPED_LENGTH		27

/***************************************************************************/
// Event Ring Segment Table Entry -- 16 bytes

struct XHCI_ERSTE
{
	U32		erste_addr_lo;		// Ring Segment Base Address (low)
	U32		erste_addr_hi;		// Ring Segment Base Address (high)
	U32		erste_size;			// Ring Segment Size (number of TRBs)
	U32		erste_reserved;
};

/***************************************************************************/
// Slot Context -- 32 bytes (or 64 if CSZ=1)

struct XHCI_SlotCtx
{
	U32		sc_info1;			// Route String, Speed, Context Entries
	U32		sc_info2;			// Max Exit Latency, Root Hub Port, Number of Ports
	U32		sc_tt;				// TT Hub Slot ID, TT Port Number, Interrupter Target
	U32		sc_state;			// Device Address, Slot State
	U32		sc_reserved[4];		// Pad to 32 bytes
};

// Slot Context info1 fields
#define XHCI_SCTX_SET_ROUTE(x)		((x) & 0xFFFFFU)
#define XHCI_SCTX_SET_SPEED(x)		(((x) & 0xFU) << 20)
#define XHCI_SCTX_SET_CTXENT(x)	(((x) & 0x1FU) << 27)

// Slot Context info2 fields
#define XHCI_SCTX_SET_RHPORT(x)	(((x) & 0xFFU) << 16)

/***************************************************************************/
// Endpoint Context -- 32 bytes (or 64 if CSZ=1)

struct XHCI_EPCtx
{
	U32		ep_info1;			// EP State, Mult, MaxPStreams, Interval, CErr
	U32		ep_info2;			// EP Type, Max Burst, Max Packet Size
	U32		ep_dequeue_lo;		// TR Dequeue Pointer (low) + DCS
	U32		ep_dequeue_hi;		// TR Dequeue Pointer (high)
	U32		ep_tx_info;			// Average TRB Length, Max ESIT Payload
	U32		ep_reserved[3];		// Pad to 32 bytes
};

// EP Context info1 fields
#define XHCI_EPCTX_SET_INTERVAL(x)	(((x) & 0xFFU) << 16)
#define XHCI_EPCTX_SET_CERR(x)		(((x) & 0x3U) << 1)
#define XHCI_EPCTX_SET_MULT(x)		(((x) & 0x3U) << 8)

// EP Context info2 fields
#define XHCI_EPCTX_SET_EPTYPE(x)	(((x) & 0x7U) << 3)
#define XHCI_EPCTX_SET_MAXBURST(x)	(((x) & 0xFFU) << 8)
#define XHCI_EPCTX_SET_MAXPKT(x)	(((x) & 0xFFFFU) << 16)

// EP types
#define XHCI_EP_ISOCH_OUT			1
#define XHCI_EP_BULK_OUT			2
#define XHCI_EP_INTR_OUT			3
#define XHCI_EP_CONTROL			4
#define XHCI_EP_ISOCH_IN			5
#define XHCI_EP_BULK_IN				6
#define XHCI_EP_INTR_IN			7

// EP tx_info fields
#define XHCI_EPCTX_SET_AVGTRB(x)	((x) & 0xFFFFU)
#define XHCI_EPCTX_SET_MAXESIT(x)	(((x) & 0xFFFFU) << 16)

// Dequeue pointer -- DCS bit
#define XHCI_EPCTX_DCS				0x00000001UL

/***************************************************************************/
// Input Control Context -- 32 bytes (or 64 if CSZ=1)

struct XHCI_InputCtrlCtx
{
	U32		icc_drop;			// Drop Context Flags (bits 0-31 = slots 0-31)
	U32		icc_add;			// Add Context Flags
	U32		icc_reserved[6];	// Pad to 32 bytes
};

/***************************************************************************/
// Software ring management

struct XHCI_Ring
{
	struct XHCI_TRB *	trbs;			// Virtual address of TRB array
	U32					phys;			// Physical address of TRB array
	U32					enqueue;		// Current enqueue index
	U32					dequeue;		// Current dequeue index (event ring)
	U32					size;			// Number of TRBs in ring
	U32					cycle;			// Current producer cycle bit (1 or 0)
};

/***************************************************************************/
// Per-slot tracking

struct XHCI_Slot
{
	PTR					output_ctx;		// Device Context (read by controller)
	U32					output_ctx_phy;
	PTR					input_ctx;		// Input Context (written by software)
	U32					input_ctx_phy;
	struct XHCI_Ring	transfer_ring[31];	// Transfer rings per endpoint (DCI 1-31)
	U8					enabled;
	volatile U8			LastTransfer_Done;	// Set by handler when a TransferEvent arrives for this slot
	U8					LastTransfer_CC;	// Completion code from last transfer event
	U8					pad;
};

/***************************************************************************/
// Main XHCI state -- embedded in USB2_HCDNode.hn_HCD.XHCI

struct _XHCI
{
	U32						CapLength;
	U32						OpBase;
	U32						RunBase;
	U32						DoorbellBase;

	U32						MaxSlots;
	U32						MaxPorts;
	U32						MaxIntrs;
	U32						ContextSize;		// 32 or 64 bytes
	U32						PageSize;
	U32						ScratchpadCount;

	// DCBAA
	U64 *					DCBAA;
	U32						DCBAA_Phyaddr;

	// Command Ring
	struct XHCI_Ring		CmdRing;

	// Event Ring
	struct XHCI_Ring		EvtRing;
	struct XHCI_ERSTE *		ERST;
	U32						ERST_Phyaddr;

	// Scratchpad
	U64 *					ScratchpadArray;
	U32						ScratchpadArray_Phyaddr;
	PTR *					ScratchpadPages;
	U32						ScratchpadCount2;	// Actual count for freeing

	// Per-slot tracking
	struct XHCI_Slot **		Slots;

	// Port tracking
	U8 *					PortResetChange;

	// Per-port cached speed (post-reset snapshot). The Renesas uPD720202
	// drops the PORTSC speed field back to 0 ("Undefined") after software
	// clears PRC, so we cache the speed observed during the post-reset
	// frame and use it later when SET_ADDRESS interception needs to fill
	// the Slot Context. Indexed by 1-based port number (1..MaxPorts).
	U8						PortSpeed[16];

	// Address-to-slot mapping (USB address 0-127 -> XHCI slot ID)
	U8						SlotID_ByAddress[128];

	// Signals
	struct USB2_Signal		Signal_Event;		// Event ring interrupt
	struct USB2_Signal		Signal_Command;		// Command completion

	// Command completion result
	U32						CmdResult_Code;		// Completion code from last command
	U32						CmdResult_SlotID;	// Slot ID from command completion
	U32						CmdResult_Param;	// Parameter from command completion
};

/***************************************************************************/

extern const struct HCDFunctions XHCIFunctions;

/***************************************************************************/

#endif // INC_PRIVATE_HCD_XHCI_H
