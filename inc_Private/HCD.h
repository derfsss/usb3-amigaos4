
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_HCD_H
#define INC_PRIVATE_HCD_H

/***************************************************************************/

struct HCDFunctions
{
	// -- Startup     : 0, 1, 2, 3, 4
	// -- Shutdown    : 5, 6, 7
	// -- Crash/Reset : 1, 6, 3, 4

	// 0: Preinit
	S32		( *Chip_Preinit )					( struct USB3_HCDNode * );

	// 1: Reset chip
	S32		( *Chip_Reset )						( struct USB3_HCDNode * );

	// 2: Alloc needed struct for startup / signals and so on
	S32		( *Chip_Alloc )						( struct USB3_HCDNode * );

	// 3: Initalize Chip Structures
	S32		( *Chip_Init )						( struct USB3_HCDNode * );

	// 4: Start / Enable Chip
	S32		( *Chip_Start )						( struct USB3_HCDNode * );

	// 5: Stop / Disable Chip
	S32		( *Chip_Stop )						( struct USB3_HCDNode * );

	// 6: Release Structures again
	void	( *Chip_Deinit )					( struct USB3_HCDNode * );

	// 7: Free Structures / signals and so on
	void	( *Chip_Dealloc )					( struct USB3_HCDNode * );

	// --

	U32		( *Handler_Interrupt )				( struct ExceptionContext *, struct ExecBase *, PTR );
	U32		( *Handler_Reset )					( struct ExceptionContext *, struct ExecBase *, PTR );
	void	( *Handler_HCD )					( struct USB3_HCDNode *, U32 );

	// -- Root HUB Functions --

	S32		( *Port_Clr_Enable )				( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_Suspend )				( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_Power )					( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_Indicator )				( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_Connection_Chg )		( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_Enable_Chg )			( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_Suspend_Chg )			( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_OverCurrent_Chg )		( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Clr_Reset_Chg )				( struct USB3_HCDNode *hn, U32 port );

	S32		( *Port_Set_Enable )				( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Set_Suspend )				( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Set_Reset )					( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Set_Power )					( struct USB3_HCDNode *hn, U32 port );
	S32		( *Port_Set_Indicator )				( struct USB3_HCDNode *hn, U32 port );

	U32		( *Port_Get_Status )				( struct USB3_HCDNode *hn, U32 port );

	// -- Buffer Functions --

	S32		( *Control_Build )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Control_Add )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Control_Remove )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	U32		( *Control_Length )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );

	S32		( *Interrupt_Build )				( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Interrupt_Add )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Interrupt_Remove )				( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	U32		( *Interrupt_Length )				( struct USB3_HCDNode *hn, struct RealRequest *ioreq );

	S32		( *Bulk_Build )						( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Bulk_Add )						( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Bulk_Remove )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	U32		( *Bulk_Length )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );

	S32		( *Isochronous_Build )				( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Isochronous_Add )				( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Isochronous_Remove )				( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	U32		( *Isochronous_Length )				( struct USB3_HCDNode *hn, struct RealRequest *ioreq );

	// -- Check if IORequest is done
	S32		( *Transfer_Check )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
	void	( *Transfer_Free )					( struct USB3_HCDNode *hn, struct RealRequest *ioreq );
};

#define PCI_READBYTE(x)						hn->HCD_ReadByte(hn,x)
#define PCI_READWORD(x)						hn->HCD_ReadWord(hn,x)
#define PCI_READLONG(x)						hn->HCD_ReadLong(hn,x)
#define PCI_WRITEBYTE(x,y)					hn->HCD_WriteByte(hn,x,y)
#define PCI_WRITEWORD(x,y)					hn->HCD_WriteWord(hn,x,y)
#define PCI_WRITELONG(x,y)					hn->HCD_WriteLong(hn,x,y)

/***************************************************************************/

struct USB3_HCDNode
{
	struct USB3_Node				hn_Node;
	struct USBBase *				hn_USBBase;

	// -- 
	U32								hn_StructID;
	S32								hn_Locks;
	U16								hn_Detach;
	U16								hn_FreeMe;
	struct USB3_TaskNode *			hn_Task; 
//	struct USB3_ASync *				hn_ASync;
	// --


//	struct USB3_TaskNode *			hn_TaskPtr;
	USB3_ID							hn_NotifyID;
	U32								hn_HCDType;
	U32								hn_HCDIndex;
	U32								hn_HCDInterruptNr;
	U32								hn_MaxSpeed;
	U32								hn_PCIDevIOBase;

//	struct USB3_Header				hn_Drivers;
//	STR								hn_HCDName;
	struct PCIDevice *				hn_PCIDevice;
	U32								hn_PCIDevLock;
	struct PCIResourceRange *		hn_PCIDevResource;
//	struct Task *					hn_Task;
//	STR								hn_TaskName;
//	struct Task *					hn_TaskFinishing;

	// ---

	struct USB3_MsgPort				hn_Abort_MsgPort;

	struct USB3_MsgPort				hn_Begin_MsgPort;

	#ifdef DO_DEBUG
	U32								hn_Delay_InUse;
	#endif

	struct USB3_MsgPort				hn_Delay_MsgPort;
	struct TimeRequest				hn_Delay_IOReq;

	struct USB3_MsgPort				hn_WatchDog_MsgPort;

	// ---

	struct USB3_EndPointNode **		hn_EndPoint_Array;
	struct RealFunctionNode **		hn_Function_Array;
	struct USB3_Header				hn_Active_Transfer_List;

	// ---

//	U16								hn_HCDInterruptPad;
//	struct USB3_Header				hn_HCDInterruptList;		// RootHub

	// ---

	struct Interrupt				hn_Reset_Interrupt;
	S32								hn_Reset_Interrupt_Added;


	// ---

//	PTR								hn_SuperState;
	struct USB3_HUBDesc *			hn_HUB_Desc;				// Root HUB
	struct USB3_ASync				hn_HUB_ASync;				// 
	struct USB3_Header				hn_HUB_Interrupts;			// Root HUB
	struct USB3_Signal				hn_HUB_Signal;				// Root HUB
	U32								hn_HUB_NumPorts;			// Set by HCD driver
	U32								hn_HUB_ConfigNr;

//	U8								hn_Running;
//	U8								hn_paddd11;
//	U8								hn_Is_ISP1561;

	// --

	U8							( *	HCD_ReadByte )				( struct USB3_HCDNode *node, U32 addr );
	U16							( *	HCD_ReadWord )				( struct USB3_HCDNode *node, U32 addr );
	U32							( *	HCD_ReadLong )				( struct USB3_HCDNode *node, U32 addr );
	void						( *	HCD_WriteByte )				( struct USB3_HCDNode *node, U32 addr, U8 val );
	void						( *	HCD_WriteWord )				( struct USB3_HCDNode *node, U32 addr, U16 val );
	void						( *	HCD_WriteLong )				( struct USB3_HCDNode *node, U32 addr, U32 val );

	U32								HCD_Mask;					// Signal HCD need.. eg. EHCI
	struct USB3_Signal				HCD_Restart_Chip;			// HCD Crashed .. we need to reinit
	struct HCDFunctions				HCD_Functions;
	struct Interrupt				HCD_Interrupt;
	S32								HCD_Interrupt_Added;

	union
	{
		struct _UHCI				UHCI;
		struct _OHCI				OHCI;
		struct _EHCI				EHCI;
		struct _XHCI				XHCI;
	}								hn_HCD;

};

/***************************************************************************/

#endif // INC_PRIVATE_HCD_H
