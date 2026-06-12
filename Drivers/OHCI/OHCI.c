
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
** Copyright (c) 2001 The NetBSD Foundation, Inc.
** All rights reserved.
**
** This code is derived from software contributed to The NetBSD Foundation
** by Lennart Augustsson (lennart@augustsson.net).
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. All advertising materials mentioning features or use of this software
**    must display the following acknowledgement:
**        This product includes software developed by the NetBSD
**        Foundation, Inc. and its contributors.
** 4. Neither the name of The NetBSD Foundation nor the names of its
**    contributors may be used to endorse or promote products derived
**    from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
** ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

// --

#include "usb3_all.h"
#include "OHCI.h"
 
// --

SEC_RODATA const struct HCDFunctions OHCIFunctions =
{
	// -- Chip

	.Chip_Preinit				= OHCI_Chip_Preinit,
	.Chip_Reset					= OHCI_Chip_Reset,
	.Chip_Alloc					= OHCI_Chip_Alloc,
	.Chip_Init					= OHCI_Chip_Init,
	.Chip_Start					= OHCI_Chip_Start,
	.Chip_Stop					= OHCI_Chip_Stop,
	.Chip_Deinit				= OHCI_Chip_Deinit,
	.Chip_Dealloc				= OHCI_Chip_Dealloc,

	// -- Handlers

	.Handler_HCD				= OHCI_Handler_HCD,
	.Handler_Reset				= OHCI_Handler_Reset,
	.Handler_Interrupt			= OHCI_Handler_Interrupt,

	// -- IORequest Transfers

	.Transfer_Check				= OHCI_Transfer_Check,
	.Transfer_Free				= OHCI_Transfer_Free,

	// -- Port Functions

	.Port_Clr_Enable			= OHCI_Port_Clr_Enable,
	.Port_Clr_Suspend			= OHCI_Port_Clr_Suspend,
	.Port_Clr_Power				= OHCI_Port_Clr_Power,
	.Port_Clr_Indicator			= OHCI_Port_Clr_Indicator,

	.Port_Clr_Enable_Chg		= OHCI_Port_Clr_Enable_Chg,
	.Port_Clr_Suspend_Chg		= OHCI_Port_Clr_Suspend_Chg,
	.Port_Clr_OverCurrent_Chg	= OHCI_Port_Clr_OverCurrent_Chg,
	.Port_Clr_Connection_Chg	= OHCI_Port_Clr_Connect_Chg,
	.Port_Clr_Reset_Chg			= OHCI_Port_Clr_Reset_Chg,

	.Port_Get_Status			= OHCI_Port_Get_Status,

	.Port_Set_Enable			= OHCI_Port_Set_Enable,
	.Port_Set_Suspend			= OHCI_Port_Set_Suspend,
	.Port_Set_Reset				= OHCI_Port_Set_Reset,
	.Port_Set_Power				= OHCI_Port_Set_Power,
	.Port_Set_Indicator			= OHCI_Port_Set_Indicator,

	// -- Control / Interrupt / Bulk / Isochronous
	
	.Bulk_Add					= OHCI_Bulk_Add,
	.Bulk_Build					= OHCI_Bulk_Build,
	.Bulk_Length				= OHCI_Bulk_Length,
	.Bulk_Remove				= OHCI_Bulk_Remove,

	.Control_Add				= OHCI_Control_Add,
	.Control_Build				= OHCI_Control_Build,
	.Control_Length				= OHCI_Control_Length,
	.Control_Remove				= OHCI_Control_Remove,

	.Interrupt_Add				= OHCI_Interrupt_Add,
	.Interrupt_Build			= OHCI_Interrupt_Build,
	.Interrupt_Length			= OHCI_Interrupt_Length,
	.Interrupt_Remove			= OHCI_Interrupt_Remove,

	// S32		( *Isochronous_Build )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
	// void	( *Isochronous_Add )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
	// void	( *Isochronous_Remove )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
	// U32		( *Isochronous_Length )				( struct USB2_HCDNode *hn, struct RealRequest *ioreq );

};

// --
