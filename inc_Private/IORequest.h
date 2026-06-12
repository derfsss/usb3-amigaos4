
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_REQUEST_H
#define INC_PRIVATE_REQUEST_H

/***************************************************************************/

struct __ohci
{
	struct OHCI_ED *		ED_Header;
	struct OHCI_ED *		Start_Header;
	U32						Start_Slot;
};

#if 0

struct __uhci
{
	struct uhci_soft_qh *	UHCI_QHHeader;
};

#endif

struct __ehci
{
	struct EHCI_QH *		QH_Header;
	struct EHCI_QH *		Start_Header;
	U32						Start_Slot;
};

// One bounce-buffer chunk of a chained bulk TD. The chunk table lives in
// a MEMID_HCD_4k allocation pointed to by __xhci.BounceTable.

struct XHCI_BounceChunk
{
	PTR						Virt;				// Chunk virtual address (MEMID_HCD_20k)
	U32						Phy;				// Chunk physical address
	U32						Len;				// Bytes of payload in this chunk
};

struct __xhci
{
	U32						SlotID;				// XHCI slot for this transfer
	U32						DCI;				// Device Context Index (endpoint)
	U32						Completed;			// Set by Handler_HCD on Transfer Event
	U32						CompletionCode;		// XHCI completion code
	U32						Residual;			// Residual bytes from event
	PTR						DataBuffer;			// DMA bounce buffer (virtual)
	U32						DataBufferPhy;		// Physical address
	U32						DataBufferLen;		// Requested length
	U32						DataBufferPool;		// MEMID pool DataBuffer came from
	PTR						BounceTable;		// Bulk: XHCI_BounceChunk[] (MEMID_HCD_4k)
	U32						BounceTablePhy;		// Physical address of the table allocation
	U32						BounceCnt;			// Number of chunks in BounceTable
	U32						EventData;			// 1 = TD ends in an Event Data TRB, so the
												//     Transfer Event length is EDTLA (bytes
												//     TRANSFERRED), not a residual
};

/*[ Real Private Struct ]***************************************************/

enum IORStat
{
	IORS_Unset,			// 0 : hmm Error?
	IORS_User,			// 1 : User controlled
	IORS_HCD,			// 2 : Gived to HCD, processing it
	IORS_HCD_Queued,	// 3 : HCD have queued it
	IORS_HCD_Active,	// 4 : HCD have give it to Hardware
};

struct RealRequest
{
	struct USB3_IORequest			req_Public;					// At the moment must be first, todo: do same as EPResource or ?

	// -- 
	U32								req_StructID;
	S32								req_Locks;
	U16								req_Detach;
	U16								req_FreeMe;
//	struct USB3_TaskNode *			req_Task;
//	struct USB3_ASync *				req_ASync;
	// --

	struct RealFunctionNode *		req_Function;
	struct USB3_EndPointNode *		req_EndPoint;
	struct USB3_MsgPort				req_MsgPort;
	struct TimeRequest				req_TimerIOReq;
	enum IORStat					req_PublicStat;

	U8								req_TimerAdded;
	U8								req_DoingDestall;
	U8								req_RetryCount;
	U8								req_RetryMax;

	#if 0
//	U8								rr_HCDControlled;			// HCD has control, need to Reply it
//	U8								rr_Pad[3];

	U32								rr_DMASize;
	U32								rr_DMAFlags;

	U8								rr_MsgPortUsed;
	U8								rr_HCDActive;				// The HCD has made it Active
	U8								rr_Pad[3];

	#ifdef DO_DEBUG
	STR								rr_TaskName;
	#endif

	#endif

	union
	{
		struct __ohci				OHCI;
//		struct __uhci				UHCI;
		struct __ehci				EHCI;
		struct __xhci				XHCI;
	}								req_HCD;
};

/***************************************************************************/

#endif // INC_PRIVATE_REQUEST_H
