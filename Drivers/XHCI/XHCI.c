
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

SEC_RODATA const struct HCDFunctions XHCIFunctions =
{
	// -- Chip Lifecycle

	.Chip_Preinit			= XHCI_Chip_Preinit,
	.Chip_Reset				= XHCI_Chip_Reset,
	.Chip_Alloc				= XHCI_Chip_Alloc,
	.Chip_Init				= XHCI_Chip_Init,
	.Chip_Start				= XHCI_Chip_Start,
	.Chip_Stop				= XHCI_Chip_Stop,
	.Chip_Deinit			= XHCI_Chip_Deinit,
	.Chip_Dealloc			= XHCI_Chip_Dealloc,

	// -- Handlers

	.Handler_HCD			= XHCI_Handler_HCD,
	.Handler_Reset			= XHCI_Handler_Reset,
	.Handler_Interrupt		= XHCI_Handler_Interrupt,

	// -- IORequest Transfers

	.Transfer_Check			= XHCI_Transfer_Check,
	.Transfer_Free			= XHCI_Transfer_Free,
	.Function_Detach		= XHCI_Function_Detach,

	// -- Port Functions

	.Port_Clr_Connection_Chg = XHCI_Port_Clr_Connect_Chg,
	.Port_Clr_Reset_Chg	= XHCI_Port_Clr_Reset_Chg,

	.Port_Get_Status		= XHCI_Port_Get_Status,

	.Port_Set_Power			= XHCI_Port_Set_Power,
	.Port_Set_Reset			= XHCI_Port_Set_Reset,

	// -- Control / Interrupt / Bulk

	.Control_Build			= XHCI_Control_Build,
	.Control_Add			= XHCI_Control_Add,
	.Control_Length			= XHCI_Control_Length,
	.Control_Remove			= XHCI_Control_Remove,

	.Interrupt_Build		= XHCI_Interrupt_Build,
	.Interrupt_Add			= XHCI_Interrupt_Add,
	.Interrupt_Length		= XHCI_Interrupt_Length,
	.Interrupt_Remove		= XHCI_Interrupt_Remove,

	.Bulk_Build				= XHCI_Bulk_Build,
	.Bulk_Add				= XHCI_Bulk_Add,
	.Bulk_Length			= XHCI_Bulk_Length,
	.Bulk_Remove			= XHCI_Bulk_Remove,
};

// --
