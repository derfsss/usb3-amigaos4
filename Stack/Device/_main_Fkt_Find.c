
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void _myFKT_GetTail( struct USBBase *usbbase, struct RealFunctionNode **ptr_fn )
{
struct RealFunctionNode *fn;
struct USB3_Node *n;
PTR f;

	// --
	// Scan
	//  Functions from Tail

	f = NULL;
	n = usbbase->usb_Fkt_Header.uh_Tail;

	if ( n )
	{
		fn = n->un_Data;

		if ( fn )
		{
			f = fn;
		}
	}

	*ptr_fn = f;

	// --
}

// --

SEC_CODE static void _myFKT_GetPrev( struct RealFunctionNode **ptr_fn )
{
struct RealFunctionNode *fn;
struct USB3_Node *n;
PTR f;

	// --
	// Scan
	//  Functions from Tail

	fn = *ptr_fn;

	f  = NULL;
	n  = fn->fkt_GlobalNode.un_Prev;

	if ( n )
	{
		fn = n->un_Data;

		if ( fn )
		{
			f = fn;
		}
	}
	else
	{
		f = fn;
	}

	*ptr_fn = f;

	// --
}

/* -- Main Interface -- */

static struct USB3_Function * VARARGS68K _main_Fkt_FindTags( struct USB3IFace *Self, ... )
{
va_list ap;
PTR fn;

//	USBERROR( "USB3 Stack : _main_FindFunctionTags" );

	va_start( ap, Self );

	fn = Self->USB3_Fkt_FindList( va_getlinearva( ap, struct TagItem * ));

	va_end( ap );

	return( fn );
}

// --

static struct USB3_Function *_main_Fkt_FindList( struct USB3IFace *Self, struct TagItem *taglist )
{
struct RealFunctionNode *fn;
struct USBBase *usbbase;
struct TagItem *tag;
S32 DoSubClass;
S32 SeeClaimed;
S32 DoClaimed;
S32 DoDevice;
S32 DoVendor;
S32 SubClass;
S32 DoIndex;
S32 DoClass;
U16 Device;
U16 Vendor;
PTR retval;
U32 Index;
S32 found;
S32 Class;
U32 cnt;

	usbbase = (PTR) Self->Data.LibBase;
	USBERROR( "USB3 Stack : _main_FindFunctionList" );

	retval = NULL;

	// -- Parse Tags

	DoVendor	= FALSE;
	DoDevice	= FALSE;
	DoClass		= FALSE;
	DoSubClass	= FALSE;
	DoClaimed	= FALSE;
	DoIndex		= FALSE;

	// --

	Vendor		= 0;
	Device		= 0;
	Class		= 0;
	SubClass	= 0;
	SeeClaimed	= FALSE;
	Index		= 0;

	// --

	while(( tag = MISC_NEXTTAGITEM( & taglist )))
	{
		switch ( tag->ti_Tag )
		{
			case USB3Tag_Find_VendorID:
			{
				USBINFO( "Find VendorID  : $%04lx", tag->ti_Data );
				Vendor = tag->ti_Data;
				DoVendor = TRUE;
				break;
			}

			case USB3Tag_Find_DeviceID:
			{
				USBINFO( "Find DeviceID : $%04lx", tag->ti_Data );
				Device = tag->ti_Data;
				DoDevice = TRUE;
				break;
			}

			case USB3Tag_Find_Class:
			{
				USBINFO( "Find Class     : %ld", tag->ti_Data );
				Class = tag->ti_Data;
				DoClass = TRUE;
				break;
			}

			case USB3Tag_Find_SubClass:
			{
				USBINFO( "Find SubClass  : %ld", tag->ti_Data );
				SubClass = tag->ti_Data;
				DoSubClass = TRUE;
				break;
			}

			case USB3Tag_Find_SeeClaimed:
			{
				USBINFO( "Find SeeClaimed: %s", ( tag->ti_Data ) ? "Yes" : "No" );
				SeeClaimed = ( tag->ti_Data ) ? TRUE : FALSE ;
				DoClaimed = TRUE;
				break;
			}

			case USB3Tag_Find_Index:
			{
				USBINFO( "Find Index     : %ld", tag->ti_Data );
				Index = tag->ti_Data;
				DoIndex = TRUE;
				break;
			}

			default:
			{
				USBINFO( "Unknown Tag    : $%04lx:%04lx", tag->ti_Tag, tag->ti_Data );
				break;
			}
		}
	}

	// -- Scan

	SEMAPHORE_OBTAIN( & usbbase->usb_Fkt_Semaphore );

	cnt = 0;

	// Functions are added to Head of list
	// so scan from bottom up so Index are more reliable
	_myFKT_GetTail( usbbase, & fn );

	while( fn )
	{
//		USBERROR( "USB3 Stack : _main_FindFunctionList : FN    %p : FktClass %lu", fn, fn->fkt_Class );

		found = TRUE;

		// Always Ignore Detached Devices
		if ( fn->fkt_Detach )
		{
			USBERROR( "fkt Detach" );
			found = FALSE;
		}

		// Check Vendor ID
		if ( DoVendor )
		{
			if ( LE_SWAP16( fn->fkt_DeviceDescriptor->VendorID ) != Vendor )
			{
//				USBERROR( "fkt Vendor : $%04lx", LE_SWAP16( fn->fkt_DeviceDescriptor->VendorID  ));
				found = FALSE;
			}
		}

		// Check Device ID
		if ( DoDevice )
		{
			if ( LE_SWAP16( fn->fkt_DeviceDescriptor->DeviceID ) != Device )
			{
//				USBERROR( "fkt Device : $%04lx", LE_SWAP16( fn->fkt_DeviceDescriptor->DeviceID  ));
				found = FALSE;
			}
		}

		// Check Interface Class
		if ( DoClass )
		{
			if ( fn->fkt_Class != Class )
			{ 
//				USBERROR( "fkt Class : %lu", (U32) fn->fkt_Class );
				found = FALSE;
			}
		}

		// Check Interface SubClass
		if ( DoSubClass )
		{
			if ( fn->fkt_SubClass != SubClass )
			{ 
//				USBERROR( "fkt SubClass : %lu", (U32) fn->fkt_SubClass );
				found = FALSE;
			}
		}

		// Check See Claimed Devices
		if ( DoClaimed )
		{
			if (( ! SeeClaimed ) && ( fn->fkt_Owner ))
			{
//				USBERROR( "fkt Claimed" );
				found = FALSE;
			}
		}

		// Did we find it?
		if ( found )
		{
			if ( DoIndex )
			{
				if ( cnt == Index )
				{
					retval = fn;
					break;
				}
				else
				{
					cnt++;
				}
			}
			else
			{
				retval = fn;
				break;
			}
		}

		_myFKT_GetPrev( & fn );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Fkt_Semaphore );

	return( retval );
}

// --
