
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"

// --

#include "HCD__RW_BE.c"
#include "HCD__RW_LE.c"
#include "HCD__RW_PCI.c"

// --

struct intern
{
	U32 idx;
	U32 uhci;
	U32 ohci;
	U32 ehci;
	U32 xhci;
};

enum _FType1
{
	FT1_Dummy,
	FT1_PCI,
	FT1_Onboard,
	FT1_Last,
};

enum _FType2
{
	FT2_Dummy,
	#ifdef DRV_UHCI
	FT2_UHCI,
	#endif
	#ifdef DRV_OHCI
	FT2_OHCI,
	#endif
	#ifdef DRV_EHCI
	FT2_EHCI,
	#endif
	#ifdef DRV_XHCI
	FT2_XHCI,
	#endif
	FT2_Last
};

enum _FEndian
{
	FE_Dummy,
	FE_Big,
	FE_Little,
	FE_Last
};

struct _FindStruct
{
	enum _FType1		fs_Type1;		// Onboard / PCI / ??
	enum _FType2		fs_Type2;
	enum _FEndian		fs_Endian;
	enum USB2HCDType	fs_HCDType;
	U32					fs_Machine;		// 
	U32					fs_IOBase;		// For Onboard or PCI
	U32					fs_IntNr;		// For Onboard only
};

// Note: EHCI before OHCI/UHCI
static struct _FindStruct _FindList[] =
{
	#ifdef DRV_XHCI

{	FT1_PCI,		FT2_XHCI, 0,			HCDTYPE_XHCI,	0,						0x000c0330,	0 },

	#endif

	#ifdef DRV_EHCI

{	FT1_Onboard,	FT2_EHCI, FE_Little,	HCDTYPE_P5020,	MACHINETYPE_X5000_20,	0xfe210100, 16 + 16 + 28 },
{	FT1_Onboard,	FT2_EHCI, FE_Little,	HCDTYPE_P5040,	MACHINETYPE_X5000_40,	0xfe210100, 16 + 16 + 28 },
{	FT1_Onboard,	FT2_EHCI, FE_Little,	HCDTYPE_P5020,	MACHINETYPE_X5000_20,	0xfe211100, 16 + 16 + 29 },
{	FT1_Onboard,	FT2_EHCI, FE_Little,	HCDTYPE_P5040,	MACHINETYPE_X5000_40,	0xfe211100, 16 + 16 + 29 },
{	FT1_Onboard,	FT2_EHCI, FE_Little,	HCDTYPE_TABOR,	MACHINETYPE_A1222,		0xffe22100, 16 + 16 + 29 },
{	FT1_Onboard,	FT2_EHCI, FE_Little,	HCDTYPE_TABOR,	MACHINETYPE_A1222,		0xffe23100, 16 + 16 + 30 },
{	FT1_Onboard,	FT2_EHCI, FE_Little,	HCDTYPE_SAM460,	MACHINETYPE_SAM460,		0xe20d0400, 16 + 93 },
{	FT1_PCI,		FT2_EHCI, 0,			HCDTYPE_EHCI,	0,						0x000c0320,	0 },

	#endif

	#ifdef DRV_OHCI

{	FT1_Onboard,	FT2_OHCI, FE_Big,		HCDTYPE_SAM440,	MACHINETYPE_SAM440,		0xef601000, 16 + 40 },
{	FT1_Onboard,	FT2_OHCI, FE_Little,	HCDTYPE_SAM460,	MACHINETYPE_SAM460,		0xe20d0000, 16 + 94 },
{	FT1_PCI,		FT2_OHCI, FE_Little,	HCDTYPE_OHCI,	0,						0x000c0310,	0 },

	#endif

	#ifdef DRV_UHCI

{	FT1_PCI,		FT2_UHCI, 0,			HCDTYPE_UHCI,	0,						0x000c0300,	0 },

	#endif

{	FT1_Last, 0, 0, 0, 0, 0, 0 },
};

// --

static void _do_Onboard( struct USBBase *usbbase, struct intern *in, struct _FindStruct *fs )
{
struct USB2_HCDNode *node;

	TASK_NAME_ENTER( "Controllers_Find : _do_Onboard" );

	// --

	USBDEBUG( "Find Machine %ld", fs->fs_Machine );
	
	if ( fs->fs_Machine == usbbase->usb_MachineType )
	{
		USBDEBUG( "Find Machine Found" );
	}
	else
	{
		USBDEBUG( "Find Machine Skip" );
		goto bailout;
	}

	// -- Create Node

	node = HCD_ALLOC( fs->fs_HCDType );

	USBDEBUG( "Find Node %p", node );

	if ( ! node )
	{
		USBERROR( "Error allocating EHCI HCD Node" );
		goto bailout;
	}

	NODE_ADDTAIL( & usbbase->usb_HCDHeader, node );

	// --

	node->hn_PCIDevIOBase 	= fs->fs_IOBase;
	node->hn_HCDInterruptNr = fs->fs_IntNr;

	/**/ if ( fs->fs_Endian == FE_Little )
	{
		node->HCD_ReadByte		= LE_ReadByte;
		node->HCD_ReadWord		= LE_ReadWord;
		node->HCD_ReadLong		= LE_ReadLong;
		node->HCD_WriteByte 	= LE_WriteByte;
		node->HCD_WriteWord		= LE_WriteWord;
		node->HCD_WriteLong		= LE_WriteLong;
	}
	else if ( fs->fs_Endian == FE_Big )
	{
		node->HCD_ReadByte		= BE_ReadByte;
		node->HCD_ReadWord		= BE_ReadWord;
		node->HCD_ReadLong		= BE_ReadLong;
		node->HCD_WriteByte 	= BE_WriteByte;
		node->HCD_WriteWord		= BE_WriteWord;
		node->HCD_WriteLong		= BE_WriteLong;
	}
	else
	{
		USBPANIC( "HCD: Unknown fs_Endian %ld", fs->fs_Endian );
	}

	if ( 0 )
	{
		// Well do nothing
	}

	#ifdef DRV_EHCI
	else if ( fs->fs_Type2 == FT2_EHCI )
	{
		in->ehci++;

		node->hn_HCDIndex		= in->idx++;
		node->hn_MaxSpeed		= USBSPEED_High;

		MEM_COPY( (PTR) & EHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
	}
	#endif

	#ifdef DRV_OHCI
	else if ( fs->fs_Type2 == FT2_OHCI )
	{
		in->ohci++;

		node->hn_HCDIndex		= in->idx++;
		node->hn_MaxSpeed		= USBSPEED_Full;

		MEM_COPY( (PTR) & OHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
	}
	#endif
	
	#ifdef DRV_UHCI
	else if ( fs->fs_Type2 == FT2_UHCI )
	{
		in->uhci++;

		node->hn_HCDIndex		= in->idx++;
		node->hn_MaxSpeed		= USBSPEED_Full;

		MEM_COPY( (PTR) & UHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
	}
	#endif

	#ifdef DRV_XHCI
	else if ( fs->fs_Type2 == FT2_XHCI )
	{
		in->xhci++;

		node->hn_HCDIndex		= in->idx++;
		node->hn_MaxSpeed		= USBSPEED_Super;

		MEM_COPY( (PTR) & XHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
	}
	#endif

bailout:

	TASK_NAME_LEAVE();
}

// --

static void _do_PCI( struct USBBase *usbbase, struct intern *in, struct _FindStruct *fs )
{
struct PCIResourceRange *range;
struct USB2_HCDNode *node;
struct PCIDevice *pcidev;
struct PCIIFace *IPCI;
U32 mask;
U32 idx;
U32 cnt;

	TASK_NAME_ENTER( "Controllers_Find : _do_PCI" );

	IPCI = usbbase->usb_IPCI;

	usbbase->usb_IExec->DebugPrintF( "USB2: _do_PCI: IPCI=%p, searching class=0x%06lx\n",
		IPCI, fs->fs_IOBase );

	idx = 0;

	while( TRUE )
	{
		// --

		usbbase->usb_IExec->DebugPrintF( "USB2: _do_PCI: FindDeviceTags(class=0x%06lx, mask=0x%08lx, idx=%ld)\n",
			fs->fs_IOBase, 0x00ffffff, idx );

		pcidev = IPCI->FindDeviceTags(
			FDT_Class,      fs->fs_IOBase,	// HCD class
			FDT_ClassMask,  0x00ffffff,
			FDT_Index,      idx++,
			TAG_END
		);

		usbbase->usb_IExec->DebugPrintF( "USB2: _do_PCI: FindDeviceTags returned pcidev=%p\n", pcidev );

		if ( ! pcidev )
		{
			// That was the last one (if any)
			break;
		}

		// --

		node = HCD_ALLOC( fs->fs_HCDType );
	
		USBDEBUG( "Find Node %p", node );

		if ( ! node )
		{
			USBERROR( "Error allocating EHCI HCD Node" );
			// Out of mem can't add more
			break;
		}

		NODE_ADDTAIL( & usbbase->usb_HCDHeader, node );

		// --

		node->hn_PCIDevLock = pcidev->Lock( PCI_LOCK_EXCLUSIVE );
	
		if ( ! node->hn_PCIDevLock )
		{
			USBERROR( "Error locking PCI device" );
			continue;
		}

		// --- Enable IO, Memory, Master

		mask  = pcidev->ReadConfigWord( PCI_COMMAND );
		mask |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
		pcidev->WriteConfigWord( PCI_COMMAND, mask );

		// ---

		range = NULL;

		for( cnt=0 ; cnt<5 && range == NULL ; cnt++ )
		{
			range = pcidev->GetResourceRange( cnt );
		}

		if ( ! range )
		{
			USBERROR( "Error getting PCI Resource" );
			continue;
		}

		node->hn_PCIDevResource = range;
		node->hn_PCIDevIOBase = range->BaseAddress;

		usbbase->usb_IExec->DebugPrintF( "USB2: _do_PCI: BAR BaseAddress=0x%08lx, Flags=0x%08lx, Size=0x%08lx\n",
			range->BaseAddress, range->Flags, range->Size );

		if ( ! node->hn_PCIDevIOBase )
		{
			USBERROR( "Error Base Address not found" );
			continue;
		}

		node->hn_HCDInterruptNr = pcidev->MapInterrupt();

		usbbase->usb_IExec->DebugPrintF( "USB2: _do_PCI: IRQ=%ld, I/O=%s\n",
			node->hn_HCDInterruptNr, (range->Flags & PCI_RANGE_IO) ? "PIO" : "MMIO" );

		// --

		if ( range->Flags & PCI_RANGE_IO )
		{
			node->HCD_ReadByte  	= PCI_ReadByte;
			node->HCD_ReadWord  	= PCI_ReadWord;
			node->HCD_ReadLong  	= PCI_ReadLong;
			node->HCD_WriteByte 	= PCI_WriteByte;
			node->HCD_WriteWord 	= PCI_WriteWord;
			node->HCD_WriteLong		= PCI_WriteLong;
		}
		else
		{
			node->HCD_ReadByte		= LE_ReadByte;
			node->HCD_ReadWord		= LE_ReadWord;
			node->HCD_ReadLong		= LE_ReadLong;
			node->HCD_WriteByte 	= LE_WriteByte;
			node->HCD_WriteWord		= LE_WriteWord;
			node->HCD_WriteLong		= LE_WriteLong;
		}

		// --

		if ( 0 )
		{
			// Well do nothing
		}

		#ifdef DRV_EHCI
		else if ( fs->fs_Type2 == FT2_EHCI )
		{
			in->ehci++;

			node->hn_HCDIndex		= in->idx++;
			node->hn_MaxSpeed		= USBSPEED_High;

			MEM_COPY( (PTR) & EHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
		}
		#endif

		#ifdef DRV_OHCI
		else if ( fs->fs_Type2 == FT2_OHCI )
		{
			in->ohci++;
	
			node->hn_HCDIndex		= in->idx++;
			node->hn_MaxSpeed		= USBSPEED_Full;
	
			MEM_COPY( (PTR) & OHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
		}
		#endif

		#ifdef DRV_UHCI
		else if ( fs->fs_Type2 == FT2_UHCI )
		{
			in->uhci++;
	
			node->hn_HCDIndex		= in->idx++;
			node->hn_MaxSpeed		= USBSPEED_Full;
	
			MEM_COPY( (PTR) & UHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
		}
		#endif

		#ifdef DRV_XHCI
		else if ( fs->fs_Type2 == FT2_XHCI )
		{
			in->xhci++;
	
			node->hn_HCDIndex		= in->idx++;
			node->hn_MaxSpeed		= USBSPEED_Super;
	
			MEM_COPY( (PTR) & XHCIFunctions, & node->HCD_Functions, sizeof( struct HCDFunctions ));
		}
		#endif
	}

	TASK_NAME_LEAVE();
}

// --

SEC_CODE U32 __HCD_Controllers_Find( struct USBBase *usbbase )
{
struct ExecIFace *IExec;
struct intern in;
U32 retval;
U32 pos;

	USBDEBUG( "__HCD_Controllers_Find" );
	TASK_NAME_ENTER( "HCD : __HCD_Controllers_Find" );

	IExec	= usbbase->usb_IExec;

	IExec->DebugPrintF( "USB2: Controllers_Find: IPCI=%p MachineType=%ld\n",
		usbbase->usb_IPCI, usbbase->usb_MachineType );

	retval	= FALSE;
	pos		= 0;

	in.idx	= 0;
	in.uhci	= 0;
	in.ohci	= 0;
	in.ehci	= 0;
	in.xhci	= 0;

	while( TRUE )
	{
		USBDEBUG( "Find pos %ld", pos );

		/**/ if ( _FindList[pos].fs_Type1 == FT1_Onboard )
		{
			IExec->DebugPrintF( "USB2: [%ld] Onboard scan, machine=%ld, IOBase=0x%08lx\n",
				pos, _FindList[pos].fs_Machine, _FindList[pos].fs_IOBase );
			_do_Onboard( usbbase, & in, & _FindList[pos] );
		}
		else if ( _FindList[pos].fs_Type1 == FT1_PCI )
		{
			IExec->DebugPrintF( "USB2: [%ld] PCI scan, class=0x%06lx\n",
				pos, _FindList[pos].fs_IOBase );
			_do_PCI( usbbase, & in, & _FindList[pos] );
		}
		else if ( _FindList[pos].fs_Type1 == FT1_Last )
		{
			break;
		}

		pos++;
	}

	IExec->Disable();
	IExec->DebugPrintF( "Found %ld UHCI Controller%s\n", in.uhci, ( in.uhci == 1 ) ? "" : "s" );
	IExec->DebugPrintF( "Found %ld OHCI Controller%s\n", in.ohci, ( in.ohci == 1 ) ? "" : "s" );
	IExec->DebugPrintF( "Found %ld EHCI Controller%s\n", in.ehci, ( in.ehci == 1 ) ? "" : "s" );
	IExec->DebugPrintF( "Found %ld xHCI Controller%s\n", in.xhci, ( in.xhci == 1 ) ? "" : "s" );
	IExec->Enable();

	// There is no point in starting if no controller is found
	retval = (( in.uhci ) || ( in.ohci ) || ( in.ehci ) || ( in.xhci )) ? TRUE : FALSE;

	TASK_NAME_LEAVE();

	return( retval );
}

// --
