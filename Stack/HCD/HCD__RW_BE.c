
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE U8 BE_ReadByte( struct USB3_HCDNode *hn, U32 addr )
{
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

	__asm volatile ("sync");
	__asm volatile ("eieio");
	val = *((volatile U8 *)( pos ));

	return( val );
}

// --

SEC_CODE U16 BE_ReadWord( struct USB3_HCDNode *hn, U32 addr )
{
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

	__asm volatile ("sync");
	__asm volatile ("eieio");
	val = *((volatile U16 *)( pos ));

	return( val );
}

// --

SEC_CODE U32 BE_ReadLong( struct USB3_HCDNode *hn, U32 addr )
{
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

	__asm volatile ("sync");
	__asm volatile ("eieio");
	val = *((volatile U32 *)( pos ));

	return( val );
}

// --

SEC_CODE void BE_WriteByte( struct USB3_HCDNode *hn, U32 addr, U8 val )
{
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;

	__asm volatile ("sync");
	__asm volatile ("eieio");
	*((volatile U8 *)( pos )) = val;
}

// --

SEC_CODE void BE_WriteWord( struct USB3_HCDNode *hn, U32 addr, U16 val )
{
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;

	__asm volatile ("sync");
	__asm volatile ("eieio");
	*((volatile U16 *)( pos )) = val;
}

// --

SEC_CODE void BE_WriteLong( struct USB3_HCDNode *hn, U32 addr, U32 val )
{
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos = hn->hn_PCIDevIOBase + addr;

	__asm volatile ("sync");
	__asm volatile ("eieio");
	*((volatile U32 *)( pos )) = val;
}

// --
