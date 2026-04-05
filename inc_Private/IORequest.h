
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB2_ALL_H
#error Include "usb2_all.h" first
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

struct __xhci
{
	U32						SlotID;				// XHCI slot for this transfer
	U32						Completed;			// Set by Handler_HCD on Transfer Event
	U32						CompletionCode;		// XHCI completion code
	U32						Residual;			// Residual bytes from event
	PTR						DataBuffer;			// DMA bounce buffer (virtual)
	U32						DataBufferPhy;		// Physical address
	U32						DataBufferLen;		// Requested length
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
	struct USB2_IORequest			req_Public;					// At the moment must be first, todo: do same as EPResource or ?

	// -- 
	U32								req_StructID;
	S32								req_Locks;
	U16								req_Detach;
	U16								req_FreeMe;
//	struct USB2_TaskNode *			req_Task;
//	struct USB2_ASync *				req_ASync;
	// --

	struct RealFunctionNode *		req_Function;
	struct USB2_EndPointNode *		req_EndPoint;
	struct USB2_MsgPort				req_MsgPort;
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
