
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_MEMORY_H
#define INC_PRIVATE_MEMORY_H

/***************************************************************************/

enum MEMID
{
	// Stack buffers
	MEMID_USBConfig,
	MEMID_USBDriver,
	MEMID_USBEndPoint,
	MEMID_USBEndPointResource,
	MEMID_USBFktDriver,
	MEMID_USBFunction,
	MEMID_USBHCD,
	MEMID_USBInterfaceGroup,
	MEMID_USBInterfaceHeader,
	MEMID_USBInterfaceNode,
	MEMID_USBIORequest,
	MEMID_USBRegister,
	MEMID_USBTask,
	MEMID_USBTaskMsg,

	// HCDs buffers
	MEMID_USBSetupData,
	MEMID_HCD_4k,
	MEMID_HCD_8k,
	MEMID_HCD_20k,

	// EHCI
	MEMID_EHCI_QH,
	MEMID_EHCI_TD,

	// OHCI
	MEMID_OHCI_ED,
	MEMID_OHCI_TD,
	MEMID_OHCI_HCCA,

	// Notify
	MEMID_NotifyNode,
	MEMID_NotifyMessage,

	#if 0

	MEMID_UHCI_QH,
	MEMID_UHCI_QTD,

	MEMID_OHCI_ITD,

	MEMID_EHCI_ITD,


	MEMID_CommandMessage,
	MEMID_Log,

//	MEMID_MSDCommand,
	#endif

	MEMID_Last,
};

struct Mem_FreeNode
{
	struct Mem_FreeNode *			mfn_Next;
	U32								mfn_Addr;		// Phy Adr
};

struct Mem_HeaderNode1				// non aligned mem
{
	S32								Free_Max;
	S32								Free_Count;
	struct Mem_FreeNode *			Free_Nodes;
	S32								Total_Count;
};

struct Mem_HeaderNode2				// alligned mem
{
	struct USB3_Header				Pages;
	U32								Real_Size;
};

struct Mem_HeaderNode
{
	struct Mem_FreeNode *			mhn_Free_Nodes;
	struct USB3_Semaphore			mhn_Semaphore;
	U32								mhn_Mem_Size;
	U32								mhn_Align;		// Alignment, also used to figure out what memtype

	union
	{
		struct Mem_HeaderNode1		Normal;	
		struct Mem_HeaderNode2		Align;	
	}								Mem;
};

struct Mem_PageNode
{
	struct USB3_Node 				mpn_Node;
	S32								mpn_Free;			// Free Chunks
	S32								mpn_Total;			// Total Chunks
	PTR								mpn_Memory;			// Virtual Memory Address
	U32								mpn_Page_Adr_1;		// Start Physical Address
	U32								mpn_Page_Adr_2;		// End   Physical Address
};

/***************************************************************************/

#endif // INC_PRIVATE_MEMORY_H
