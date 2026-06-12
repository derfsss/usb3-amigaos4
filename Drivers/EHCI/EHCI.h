
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

#ifndef DRIVERS_EHCI_EHCI_H
#define DRIVERS_EHCI_EHCI_H

/***************************************************************************/

#if defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

void EHCI_Dump_Setup(	struct USB3_HCDNode *hn, struct USB3_SetupData *sd );
void EHCI_Dump_QH(		struct USB3_HCDNode *hn, struct EHCI_QH *qh, int DoSetup );
void EHCI_Dump_TD(		struct USB3_HCDNode *hn, struct EHCI_TD *td );

#define EHCI_DUMP_SETUP(x,y)	EHCI_Dump_Setup(x,y)
#define EHCI_DUMP_QH(x,y,z)		EHCI_Dump_QH(x,y,z)
#define EHCI_DUMP_TD(x,y)		EHCI_Dump_TD(x,y)

#else

#define EHCI_DUMP_SETUP(x,y)	((void)0)
#define EHCI_DUMP_QH(x,y,z)		((void)0)
#define EHCI_DUMP_TD(x,y)		((void)0)

#endif

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void	EHCI_Door_Bell_Init( struct USB3_HCDNode *hn, STR file );
SEC_CODE void	EHCI_Door_Bell_Wait( struct USB3_HCDNode *hn, STR file );

#else

SEC_CODE void	EHCI_Door_Bell_Init( struct USB3_HCDNode *hn );
SEC_CODE void	EHCI_Door_Bell_Wait( struct USB3_HCDNode *hn );

#endif

SEC_CODE void	EHCI_Bulk_Add( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE S32	EHCI_Bulk_Build( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE U32	EHCI_Bulk_Length( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	EHCI_Bulk_Remove( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	EHCI_Control_Add( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE S32	EHCI_Control_Build( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE U32	EHCI_Control_Length( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	EHCI_Control_Remove( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	EHCI_Interrupt_Add( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE S32	EHCI_Interrupt_Build( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE U32	EHCI_Interrupt_Length( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	EHCI_Interrupt_Remove( struct USB3_HCDNode *hn, struct RealRequest *ioreq );

SEC_CODE S32	EHCI_Chip_Preinit( struct USB3_HCDNode *hn );
SEC_CODE S32	EHCI_Chip_Reset( struct USB3_HCDNode *hn );
SEC_CODE S32	EHCI_Chip_Alloc( struct USB3_HCDNode *hn );
SEC_CODE S32	EHCI_Chip_Init( struct USB3_HCDNode *hn );
SEC_CODE S32	EHCI_Chip_Start( struct USB3_HCDNode *hn );
SEC_CODE S32	EHCI_Chip_Stop( struct USB3_HCDNode *hn );
SEC_CODE void	EHCI_Chip_Deinit( struct USB3_HCDNode *hn );
SEC_CODE void	EHCI_Chip_Dealloc( struct USB3_HCDNode *hn );

SEC_CODE S32	EHCI_Port_Clr_Connect_Chg( struct USB3_HCDNode *hn, U32 port );
SEC_CODE S32	EHCI_Port_Clr_Reset_Chg( struct USB3_HCDNode *hn, U32 port );
SEC_CODE void	EHCI_Port_Disown( struct USB3_HCDNode *hn, U32 port );
SEC_CODE U32	EHCI_Port_Get_Status( struct USB3_HCDNode *hn, U32 port );
SEC_CODE S32	EHCI_Port_Set_Power( struct USB3_HCDNode *hn, U32 port );
SEC_CODE S32	EHCI_Port_Set_Reset( struct USB3_HCDNode *hn, U32 port );

SEC_CODE S32	EHCI_Get_20kBuffer( struct USB3_HCDNode *hn, struct EHCI_ITD *itd, struct EHCI_TD *td, PTR data, U32 len );
SEC_CODE struct EHCI_QH *EHCI_Get_QHBuffer( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE struct EHCI_TD *EHCI_Get_TDBuffer( struct USB3_HCDNode *hn );

SEC_CODE S32	EHCI_Enough_Bandwidth( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE S32	EHCI_Slot_Find( struct USB3_HCDNode *hn, U32 interval );
SEC_CODE void	EHCI_Slot_Free( struct USB3_HCDNode *hn, U32 slot );
SEC_CODE void	EHCI_Handler_HCD( struct USB3_HCDNode *hn, U32 mask );
SEC_CODE U32	EHCI_Handler_Interrupt( struct ExceptionContext *Context, struct ExecBase *SysBase, PTR userData );
SEC_CODE U32	EHCI_Handler_Reset( struct ExceptionContext *Context, struct ExecBase *SysBase, PTR userData );
SEC_CODE void	EHCI_Periodic_Start( struct USB3_HCDNode *hn );
SEC_CODE void	EHCI_Periodic_Stop( struct USB3_HCDNode *hn );
SEC_CODE S32	EHCI_Transfer_Check( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
SEC_CODE void	EHCI_Transfer_Free( struct USB3_HCDNode *hn, struct RealRequest *ioreq );

/***************************************************************************/

#endif // DRIVERS_EHCI_EHCI_H
