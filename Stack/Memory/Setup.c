
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

struct myMemList
{
	U32		Align;		// Min Mem Alignment
	U32		Size;		// Structure byte Size
	U32		Max;		// Max free Nodes
	U32		Pre;		// Prealloc on startup
};

static const struct myMemList TheMemList[MEMID_Last] =
{
	[MEMID_USBIORequest]		= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct RealRequest ) },
	[MEMID_USBRegister]			= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct RealRegister ) },
	[MEMID_USBEndPointResource]	= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct RealEndPointResource ) },
	[MEMID_USBInterfaceGroup]	= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_InterfaceGroup ) },
	[MEMID_USBInterfaceHeader]	= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_InterfaceHeader ) },
	[MEMID_USBInterfaceNode]	= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_InterfaceNode ) },
	[MEMID_USBFunction]			= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct RealFunctionNode ) },
	[MEMID_USBEndPoint]			= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_EndPointNode ) },
	[MEMID_USBConfig]			= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_ConfigNode ) },
	[MEMID_USBDriver]			= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_DriverNode ) },
	[MEMID_USBTaskMsg]			= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_TaskMsg ) },
	// Well HCDs are only allocaterd on startup, queuing is not needed
	[MEMID_USBHCD]				= { .Max =  0, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_HCDNode ) },
	[MEMID_USBFktDriver]		= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_FktDriverNode ) },
	[MEMID_USBTask]				= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_TaskNode ) },
	[MEMID_EHCI_QH]				= { .Max = 25, .Pre = 0, .Align = EHCI_QH_ALIGN, .Size = sizeof( struct EHCI_QH ) },
	[MEMID_EHCI_TD]				= { .Max = 25, .Pre = 0, .Align = EHCI_TD_ALIGN, .Size = sizeof( struct EHCI_TD ) },
	[MEMID_OHCI_ED]				= { .Max = 25, .Pre = 0, .Align = OHCI_ED_ALIGN, .Size = sizeof( struct OHCI_ED ) },
	[MEMID_OHCI_TD]				= { .Max = 25, .Pre = 0, .Align = OHCI_TD_ALIGN, .Size = sizeof( struct OHCI_TD ) },
	[MEMID_OHCI_HCCA]			= { .Max = 25, .Pre = 0, .Align = OHCI_HCCA_ALIGN, .Size = sizeof( struct OHCI_HCCA ) },
	[MEMID_HCD_20k]				= { .Max = 25, .Pre = 0, .Align = 1024 * 4, .Size = 1024 * 20 },
	[MEMID_HCD_8k]				= { .Max = 25, .Pre = 0, .Align = 1024 * 4, .Size = 1024 * 8 },
	[MEMID_HCD_4k]				= { .Max = 25, .Pre = 0, .Align = 1024 * 4, .Size = 1024 * 4 },
	// There are not really an alignment requirement, but we need to enable alignment with a x>1 value
	[MEMID_USBSetupData]		= { .Max = 25, .Pre = 0, .Align = 2, .Size = sizeof( struct RealSetupData ) },
	[MEMID_NotifyNode]			= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_NotifyNode ) },
	[MEMID_NotifyMessage]		= { .Max = 25, .Pre = 0, .Align = 1, .Size = sizeof( struct USB3_NotifyMessage ) },
};

// --

SEC_RWDATA char *idnames[MEMID_Last] =
{
	// Stack buffers
	[MEMID_USBFunction]				= "MEMID_USBFunction",
	[MEMID_USBEndPoint]				= "MEMID_USBEndPoint",
	[MEMID_USBEndPointResource]		= "MEMID_USBEndPointResource",
	[MEMID_USBIORequest]			= "MEMID_USBIORequest",
	[MEMID_USBRegister]				= "MEMID_USBRegister",
	[MEMID_USBInterfaceGroup]		= "MEMID_USBInterfaceGroup",
	[MEMID_USBInterfaceHeader]		= "MEMID_USBInterfaceHeader",
	[MEMID_USBInterfaceNode]		= "MEMID_USBInterfaceNode",
	[MEMID_USBConfig]				= "MEMID_USBConfig",
	[MEMID_USBDriver]				= "MEMID_USBDriver",
	[MEMID_USBTaskMsg]				= "MEMID_USBTaskMsg",
	[MEMID_USBHCD]					= "MEMID_USBHCD",
	[MEMID_USBFktDriver]			= "MEMID_USBFktDriver",
	[MEMID_USBTask]					= "MEMID_USBTask",

	// HCDs buffers
	[MEMID_USBSetupData]			= "MEMID_USBSetupData",
	[MEMID_HCD_20k]					= "MEMID_HCD_20k",
	[MEMID_HCD_4k]					= "MEMID_HCD_4k",

	// EHCI
	[MEMID_EHCI_QH]					= "MEMID_EHCI_QH",
	[MEMID_EHCI_TD]					= "MEMID_EHCI_TD",
	
	// OHCI
	[MEMID_OHCI_ED]					= "MEMID_OHCI_ED",
	[MEMID_OHCI_TD]					= "MEMID_OHCI_TD",
	[MEMID_OHCI_HCCA]				= "MEMID_OHCI_HCCA",

	// Notify
	[MEMID_NotifyNode]				= "MEMID_NotifyNode",
	[MEMID_NotifyMessage]			= "MEMID_NotifyMessage",

};

SEC_CODE S32 __Memory_Setup( struct USBBase *usbbase )
{
const struct myMemList *l;
struct Mem_HeaderNode *mhn;
struct Mem_HeaderNode *h;
S32 retval;
U32 size;
U32 cnt;

	retval = FALSE;

	mhn = MEM_ALLOCVEC( sizeof( struct Mem_HeaderNode ) * MEMID_Last, TRUE );

	if ( ! mhn )
	{
		USBERROR( "Error: Allocating memory" );
		goto bailout;
	}

	usbbase->usb_Memory_Headers = mhn;

	for( cnt=0 ; cnt<MEMID_Last ; cnt++ )
	{
		l = & TheMemList[cnt] ;
		h = & mhn[cnt] ;

		size = MAX( l->Size, sizeof( struct Mem_FreeNode ) );

		// as long as we allocate 4k pages and inhibit cache we are okay
		// but if we stop that, we also need to make sure we support cpu cache line size
		// Like ..  align = MAX( l->Align , CPU Cache Line )

		SEMAPHORE_INIT( & h->mhn_Semaphore );
		h->mhn_Align		= l->Align;
		h->mhn_Mem_Size		= size;

		if ( h->mhn_Align > 1 )
		{
			// do alignment

			h->Mem.Align.Real_Size  = usbbase->usb_CPU_PageSize + size - 1;
			h->Mem.Align.Real_Size /= usbbase->usb_CPU_PageSize;
			h->Mem.Align.Real_Size *= usbbase->usb_CPU_PageSize;
		}
		else
		{
			// not aligned

			h->Mem.Normal.Free_Max = l->Max;
		}

//		node->Mem_Size		= ml->Size;
//		node->Mem_Alignment = ml->Align;
//		node->Mem_Name		= ml->Name;
//		node->MaxFree		= ml->Max;
//		node->CanFree		= (( ml->Align <= 1 ) || ( usbbase->usb_CPU_PageSize / 2 < ml->Size )) ? TRUE : FALSE;

//		node->RealSize  = usbbase->usb_CPU_PageSize + ml->Size - 1;
//		node->RealSize /= usbbase->usb_CPU_PageSize;
//		node->RealSize *= usbbase->usb_CPU_PageSize;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --
