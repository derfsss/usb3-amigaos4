
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

 // --

 #include "usb3_all.h"
 #include "UHCI.h"
 
 // --

SEC_RODATA const struct HCDFunctions UHCIFunctions =
{

	// // -- Chip

	// .Chip_Preinit			= EHCI_Chip_Preinit,
	// .Chip_Reset				= EHCI_Chip_Reset,
	// .Chip_Alloc				= EHCI_Chip_Alloc,
	// .Chip_Init				= EHCI_Chip_Init,
	// .Chip_Start				= EHCI_Chip_Start,
	// .Chip_Stop				= EHCI_Chip_Stop,
	// .Chip_Deinit			= EHCI_Chip_Deinit,
	// .Chip_Dealloc			= EHCI_Chip_Dealloc,

	// // -- Handlers

	// .Handler_HCD			= EHCI_Handler_HCD,
	// .Handler_Reset			= EHCI_Handler_Reset,
	// .Handler_Interrupt		= EHCI_Handler_Interrupt,

	// // -- IORequest Transfers

	// .Transfer_Check			= EHCI_Transfer_Check,
	// .Transfer_Free			= EHCI_Transfer_Free,

	// // -- Port Functions

	// .Port_Clr_Connection_Chg = EHCI_Port_Clr_Connect_Chg,
	// .Port_Clr_Reset_Chg		= EHCI_Port_Clr_Reset_Chg,

	// .Port_Get_Status		= EHCI_Port_Get_Status,

	// .Port_Set_Power			= EHCI_Port_Set_Power,
	// .Port_Set_Reset			= EHCI_Port_Set_Reset,

	// // -- Control / Interrupt / Bulk / Isochronous

	// .Control_Add			= EHCI_Control_Add,
	// .Control_Build			= EHCI_Control_Build,
	// .Control_Length			= EHCI_Control_Length,
	// .Control_Remove			= EHCI_Control_Remove,

	// .Interrupt_Add			= EHCI_Interrupt_Add,
	// .Interrupt_Build		= EHCI_Interrupt_Build,
	// .Interrupt_Length		= EHCI_Interrupt_Length,
	// .Interrupt_Remove		= EHCI_Interrupt_Remove,

	// .Bulk_Add				= EHCI_Bulk_Add,
	// .Bulk_Build				= EHCI_Bulk_Build,
	// .Bulk_Length			= EHCI_Bulk_Length,
	// .Bulk_Remove			= EHCI_Bulk_Remove,

	// // --

	// #if 0

	// // -- Root HUB Functions --

	// S8		( *Port_Clr_Enable )				( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Clr_Suspend )				( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Clr_Power )					( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Clr_Indicator )				( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Clr_Enable_Chg )			( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Clr_Suspend_Chg )			( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Clr_OverCurrent_Chg )		( struct USB2_HCDNode *hn, U32 port );
	// S8 		( *Port_Clr_Reset_Chg )				( struct USB2_HCDNode *hn, U32 port );

	// S8		( *Port_Set_Enable )				( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Set_Suspend )				( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Set_Reset )					( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Set_Power )					( struct USB2_HCDNode *hn, U32 port );
	// S8		( *Port_Set_Indicator )				( struct USB2_HCDNode *hn, U32 port );

	// // -- Buffer Functions --


	// S32		( *Isochronous_Build )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
	// void	( *Isochronous_Add )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
	// void	( *Isochronous_Remove )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
	// U32		( *Isochronous_Length )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );

	// #endif
};

// --
