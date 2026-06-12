
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE U8 LE_ReadByte( struct USB2_HCDNode *hn, U32 addr )
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

	pos	= hn->hn_PCIDevIOBase + addr;

	val = *((volatile U8 *)( pos ));

	return( val );
}

// --

SEC_CODE U16 LE_ReadWord( struct USB2_HCDNode *hn, U32 addr )
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

	pos	= hn->hn_PCIDevIOBase + addr;

	val = *((volatile U16 *)( pos ));
	val = LE_SWAP16( val );

	return( val );
}

// --

SEC_CODE U32 LE_ReadLong( struct USB2_HCDNode *hn, U32 addr )
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

	pos	= hn->hn_PCIDevIOBase + addr;

	val = *((volatile U32 *)( pos ));
	val = LE_SWAP32( val );

	return( val );
}

// --

SEC_CODE void LE_WriteByte( struct USB2_HCDNode *hn, U32 addr, U8 val )
{
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos	= hn->hn_PCIDevIOBase + addr;

	*((volatile U8 *)( pos )) = val;
}

// --

SEC_CODE void LE_WriteWord( struct USB2_HCDNode *hn, U32 addr, U16 val )
{
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos	= hn->hn_PCIDevIOBase + addr;
	val =  LE_SWAP16( val );

	*((volatile U16 *)( pos )) = val;
}

// --

SEC_CODE void LE_WriteLong( struct USB2_HCDNode *hn, U32 addr, U32 val )
{
U32 pos;

	#ifdef DO_DEBUG

	struct USBBase *usbbase = hn->hn_USBBase;
	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		USBPANIC( "Invalid HCD Node (%p)", hn );
	}

	#endif

	pos	= hn->hn_PCIDevIOBase + addr;
	val = LE_SWAP32( val );

	*((volatile U32 *)( pos )) = val;
}

// --
