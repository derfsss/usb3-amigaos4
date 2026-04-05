
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"

// --

static U8 PCI_ReadByte( struct USB2_HCDNode *hn, U32 addr )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
U32 pos;
U8 val;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;
	val = pd->InByte( pos );

	return( val );
}

// --

static U16 PCI_ReadWord( struct USB2_HCDNode *hn, U32 addr )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
U32 pos;
U16 val;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;
	val = pd->InWord( pos );

	return( val );
}

// --

SEC_CODE static U32 PCI_ReadLong( struct USB2_HCDNode *hn, U32 addr )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
U32 pos;
U32 val;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;
	val = pd->InLong( pos );

	return( val );
}

// --

SEC_CODE static void PCI_WriteByte( struct USB2_HCDNode *hn, U32 addr, U8 val )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;

	pd->OutByte( pos, val );
}

// --

SEC_CODE static void PCI_WriteWord( struct USB2_HCDNode *hn, U32 addr, U16 val )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;

	pd->OutWord( pos, val );
}

// --

SEC_CODE static void PCI_WriteLong( struct USB2_HCDNode *hn, U32 addr, U32 val )
{
struct PCIDevice *pd = hn->hn_PCIDevice;
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;
	pd->OutLong( pos, val );
}

// --
