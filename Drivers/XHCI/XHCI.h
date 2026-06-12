
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef DRIVERS_XHCI_XHCI_H
#define DRIVERS_XHCI_XHCI_H

/***************************************************************************/

SEC_CODE S32	XHCI_Chip_Preinit( struct USB2_HCDNode *hn );
SEC_CODE S32	XHCI_Chip_Reset( struct USB2_HCDNode *hn );
SEC_CODE S32	XHCI_Chip_Alloc( struct USB2_HCDNode *hn );
SEC_CODE S32	XHCI_Chip_Init( struct USB2_HCDNode *hn );
SEC_CODE S32	XHCI_Chip_Start( struct USB2_HCDNode *hn );
SEC_CODE S32	XHCI_Chip_Stop( struct USB2_HCDNode *hn );
SEC_CODE void	XHCI_Chip_Deinit( struct USB2_HCDNode *hn );
SEC_CODE void	XHCI_Chip_Dealloc( struct USB2_HCDNode *hn );

SEC_CODE U32	XHCI_Handler_Interrupt( struct ExceptionContext *Context, struct ExecBase *SysBase, PTR userData );
SEC_CODE U32	XHCI_Handler_Reset( struct ExceptionContext *Context, struct ExecBase *SysBase, PTR userData );
SEC_CODE void	XHCI_Handler_HCD( struct USB2_HCDNode *hn, U32 mask );

SEC_CODE S32	XHCI_Transfer_Check( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	XHCI_Transfer_Free( struct USB2_HCDNode *hn, struct RealRequest *ioreq );

SEC_CODE S32	XHCI_Port_Clr_Connect_Chg( struct USB2_HCDNode *hn, U32 port );
SEC_CODE S32	XHCI_Port_Clr_Reset_Chg( struct USB2_HCDNode *hn, U32 port );
SEC_CODE U32	XHCI_Port_Get_Status( struct USB2_HCDNode *hn, U32 port );
SEC_CODE S32	XHCI_Port_Set_Power( struct USB2_HCDNode *hn, U32 port );
SEC_CODE S32	XHCI_Port_Set_Reset( struct USB2_HCDNode *hn, U32 port );

SEC_CODE S32	XHCI_Control_Build( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	XHCI_Control_Add( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	XHCI_Control_Remove( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE U32	XHCI_Control_Length( struct USB2_HCDNode *hn, struct RealRequest *ioreq );

SEC_CODE S32	XHCI_Bulk_Build( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	XHCI_Bulk_Add( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	XHCI_Bulk_Remove( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE U32	XHCI_Bulk_Length( struct USB2_HCDNode *hn, struct RealRequest *ioreq );

SEC_CODE S32	XHCI_Interrupt_Build( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	XHCI_Interrupt_Add( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	XHCI_Interrupt_Remove( struct USB2_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE U32	XHCI_Interrupt_Length( struct USB2_HCDNode *hn, struct RealRequest *ioreq );

// -- Transfer Ring helper

SEC_CODE U32	XHCI_Ring_Enqueue_TRB( struct USB2_HCDNode *hn, struct XHCI_Ring *ring, struct XHCI_TRB *trb );

// -- Command Ring operations

SEC_CODE U32	XHCI_Cmd_EnableSlot( struct USB2_HCDNode *hn );
SEC_CODE S32	XHCI_Cmd_DisableSlot( struct USB2_HCDNode *hn, U32 slotid );
SEC_CODE S32	XHCI_Cmd_AddressDevice( struct USB2_HCDNode *hn, U32 slotid, U32 bsr );
SEC_CODE S32	XHCI_Cmd_ConfigureEndpoint( struct USB2_HCDNode *hn, U32 slotid );
SEC_CODE S32	XHCI_Cmd_EvaluateContext( struct USB2_HCDNode *hn, U32 slotid );
SEC_CODE S32	XHCI_Cmd_ResetEndpoint( struct USB2_HCDNode *hn, U32 slotid, U32 dci );
SEC_CODE S32	XHCI_Cmd_StopEndpoint( struct USB2_HCDNode *hn, U32 slotid, U32 dci );

// -- Slot management

SEC_CODE S32	XHCI_Slot_Alloc( struct USB2_HCDNode *hn, U32 slotid, U32 port, U32 usb_speed );
SEC_CODE void	XHCI_Slot_Free( struct USB2_HCDNode *hn, U32 slotid );
SEC_CODE U32	XHCI_Endpoint_DCI( U32 ep_number, U32 ep_direction );
SEC_CODE S32	XHCI_Slot_ConfigureEndpoints( struct USB2_HCDNode *hn, U32 slotid, struct RealFunctionNode *fn );

/***************************************************************************/

#endif // DRIVERS_XHCI_XHCI_H
