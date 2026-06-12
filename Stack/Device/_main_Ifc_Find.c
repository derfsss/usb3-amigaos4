
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void _myIFC_GetTail( 
	struct USBBase *usbbase, 
	struct RealFunctionNode **ptr_fn, 
	struct USB2_InterfaceGroup **ptr_ig,
	struct USB2_InterfaceHeader **ptr_ih )
{
struct USB2_InterfaceHeader *ih;
struct USB2_InterfaceGroup *ig;
struct RealFunctionNode *fn;
struct USB2_ConfigNode *cn;
struct USB2_Node *n;
PTR f;
PTR g;
PTR h;

	f = NULL;
	g = NULL;
	h = NULL;

	// --
	// Scan
	//  Functions from Tail
	//  Interfaces Group from Head
	//  Interfaces Header from Head

	n = usbbase->usb_Fkt_Header.uh_Tail;

	if ( n )
	{
		fn = n->un_Data;

		if ( fn )
		{
			cn = fn->fkt_Config_Active;

			if ( cn )
			{
				ig = cn->cfg_InterfaceGroups.uh_Head;

				if ( ig )
				{
					ih = ig->ig_Group.uh_Head;
	
					if ( ih )
					{
						f = fn;
						g = ig;
						h = ih;
					}
				}
			}
		}
	}

	// --

	*ptr_fn = f;
	*ptr_ig = g;
	*ptr_ih = h;
}

// --

SEC_CODE static void _myIFC_GetPrev( 
	struct RealFunctionNode **ptr_fn, 
	struct USB2_InterfaceGroup **ptr_ig,
	struct USB2_InterfaceHeader **ptr_ih )
{
struct USB2_InterfaceHeader *ih;
struct USB2_InterfaceGroup *ig;
struct RealFunctionNode *fn;
struct USB2_ConfigNode *cn;
struct USB2_Node *n;
PTR f;
PTR g;
PTR h;

	f  = NULL;
	g  = NULL;
	h  = NULL;
	fn = *ptr_fn;
	ig = *ptr_ig;
	ih = *ptr_ih;

	// --
	// Scan
	//  Functions from Tail
	//  Interfaces Group from Head
	//  Interfaces Header from Head

	while( TRUE )
	{
		// -- Next : Ifc Header

		ih = ih->ih_Node.un_Next;

		if ( ih )
		{
			f = fn;
			g = ig;
			h = ih;
			break;
		}

		// -- Next : Ifc Group

		ig = ig->ig_Node.un_Next;

		if ( ig )
		{
			ih = ig->ig_Group.uh_Head;

			if ( ih )
			{
				f = fn;
				g = ig;
				h = ih;
				break;
			}
		}

		// -- Next : Function

		n = fn->fkt_GlobalNode.un_Prev;

		if ( n )
		{
			fn = n->un_Data;

			if ( fn )
			{
				cn = fn->fkt_Config_Active;

				if ( cn )
				{
					ig = cn->cfg_InterfaceGroups.uh_Head;

					if ( ig )
					{
						ih = ig->ig_Group.uh_Head;

						if ( ih )
						{
							f = fn;
							g = ig;
							h = ih;
							break;
						}
					}
				}
			}
		}
		break;
	}

	// --

	*ptr_fn = f;
	*ptr_ig = g;
	*ptr_ih = h;
}

// --

/* -- Main Interface -- */

static struct USB2_Interface * VARARGS68K _main_Ifc_FindTags( struct USB2IFace *Self, ... )
{
va_list ap;
PTR ifc;

//	USBERROR( "USB2 Stack : _main_FindInterfaceTags" );

	va_start( ap, Self );

	ifc = Self->USB2_Ifc_FindList( va_getlinearva( ap, struct TagItem * ));

	va_end( ap );

	return( ifc );
}

// --

static struct USB2_Interface *_main_Ifc_FindList( struct USB2IFace *Self, struct TagItem *taglist )
{
struct USB2_InterfaceHeader *ih;
struct USB2_InterfaceGroup *ig;
struct RealFunctionNode *fn;
struct USBBase *usbbase;
struct TagItem *tag;
S32 SeeClaimed;
S32 DoSubClass;
S32 DoClaimed;
S32 DoVendor;
S32 DoDevice;
S32 SubClass;
S32 DoClass;
S32 DoIndex;
U16 Vendor;
U16 Device;
PTR retval;
U32 Index;
S32 found;
S32 Class;
U32 cnt;

	usbbase = (PTR) Self->Data.LibBase;

	USBERROR( "USB2 Stack : _main_FindInterfaceList" );

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
			case USB2Tag_Find_VendorID:
			{
				USBDEBUG( "Find VendorID  : $%04lx", tag->ti_Data );
				Vendor = tag->ti_Data;
				DoVendor = TRUE;
				break;
			}

			case USB2Tag_Find_DeviceID:
			{
				USBDEBUG( "Find DeviceID : $%04lx", tag->ti_Data );
				Device = tag->ti_Data;
				DoDevice = TRUE;
				break;
			}

			case USB2Tag_Find_Class:
			{
				USBINFO( "Find Class     : %ld", tag->ti_Data );
				Class = tag->ti_Data;
				DoClass = TRUE;
				break;
			}

			case USB2Tag_Find_SubClass:
			{
				USBINFO( "Find SubClass  : %ld", tag->ti_Data );
				SubClass = tag->ti_Data;
				DoSubClass = TRUE;
				break;
			}

			case USB2Tag_Find_SeeClaimed:
			{
				USBINFO( "Find SeeClaimed: %s", ( tag->ti_Data ) ? "Yes" : "No" );
				SeeClaimed = ( tag->ti_Data ) ? TRUE : FALSE ;
				DoClaimed = TRUE;
				break;
			}

			case USB2Tag_Find_Index:
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
	_myIFC_GetTail( usbbase, & fn, & ig, & ih );

	while(( fn ) && ( ig ) && ( ih ))
	{
		USBERROR( "USB2 Stack : _main_FindInterfaceList : FN    %p : IG    %p : IH    %p : Ifc Class %lu", fn, ig, ih, ig->ig_Class );

		found = TRUE;

		// Always Ignore Detached Devices
		if ( fn->fkt_Detach )
		{
//			USBERROR( "fkt Detach" );
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
			if ( ig->ig_Class != Class )
			{ 
//				USBERROR( "ifc Class : %lu", (U32) ig->ig_Class );
				found = FALSE;
			}
		}

		// Check Interface SubClass
		if ( DoSubClass )
		{
			if ( ig->ig_SubClass != SubClass )
			{ 
//				USBERROR( "ifc SubClass : %lu", (U32) ig->ig_SubClass );
				found = FALSE;
			}
		}

		// Check SeeClaimed Devices
		if ( DoClaimed )
		{
			if ( ! SeeClaimed )
			{
				/**/ if ( fn->fkt_Owner )
				{
//					USBERROR( "fkt Claimed" );
					found = FALSE;
				}
				else if ( ih->ih_Owner )
				{
//					USBERROR( "ifc Claimed" );
					found = FALSE;
				}
			}
		}


		// Did we find it?
		if ( found )
		{
			if ( DoIndex )
			{
				if ( cnt == Index )
				{
					retval = & ih->ih_Public ;
					break;
				}
				else
				{
					cnt++;
				}
			}
			else
			{
				retval = & ih->ih_Public ;
				break;
			}
		}

		_myIFC_GetPrev( & fn, & ig, & ih );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Fkt_Semaphore );

	return( retval );
}

// --
