
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE struct USB2_Interface_Desc *__Desc_Next_Interface( struct USBBase *usbbase, struct RealRegister *reg, struct USB2_Interface_Desc *ifcdsc )
{
struct USB2_Interface_Desc *next;
struct RealFunctionNode *fn;
struct USB2_Descriptor *dsc;
struct USB2_ConfigNode *cn;

	next = NULL;
	dsc = NULL;
	fn	= NULL;
	cn	= NULL;

	// --

	if ( REGISTER_VALID( reg ) != VSTAT_Okay )
	{
		reg = NULL;
		USBDEBUG( "Invalid Register Pointer %p", reg );
		goto bailout;
	}

	SEMAPHORE_OBTAIN( & reg->reg_Semaphore );

	// --

	fn = reg->reg_Function;

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		fn = NULL;
		USBDEBUG( "Function Lock Failed" );
		goto bailout;
	}

	// --

	cn = fn->fkt_Config_Active;

	if ( CONFIG_LOCK(cn) != LSTAT_Okay )
	{
		cn = NULL;
		USBDEBUG( "Config Lock Failed" );
		goto bailout;
	}

	dsc = (PTR) cn->cfg_Descriptor;

	if ( ! dsc )
	{
		USBDEBUG( "NULL Pointer" );
		goto bailout;
	}

	// --

	if ( dsc->DescriptorType == DSCTYPE_Configuration )
	{
		// Skip Config Descriptor
		dsc = DESC_NEXT_DESC( dsc );
	}

	// --

	if ( ifcdsc )
	{
		if ( ifcdsc->DescriptorType != DSCTYPE_Interface )
		{
			USBDEBUG( "Not an Interface Descriptor" );
			goto bailout;
		}

		// First check given Interface Descriptor is in Active Config

		while(( dsc ) && ( dsc->DescriptorType != DSCTYPE_Configuration ))
		{
			if (( dsc->DescriptorType == DSCTYPE_Interface ) && ( dsc == (PTR) ifcdsc ))
			{
//				USBDEBUG( "Found ifcdsc" );
				break;
			}
			else
			{
				dsc = DESC_NEXT_DESC( dsc );
			}
		}

		if (( ! dsc ) || ( dsc->DescriptorType == DSCTYPE_Configuration ))
		{
			// This is not an error
			goto bailout;
		}

		// Skip Interface Descriptor
		dsc = DESC_NEXT_DESC( dsc );
	}

	// Find Next Interface Descriptor

	while(( dsc ) && ( dsc->DescriptorType != DSCTYPE_Configuration ))
	{
		if ( dsc->DescriptorType == DSCTYPE_Interface )
		{
//			USBDEBUG( "Found Interface Descriptor" );
			next = (PTR) dsc;
			break;
		}
		else
		{
			dsc = DESC_NEXT_DESC( dsc );
		}
	}

bailout:

	if ( cn )
	{
		CONFIG_UNLOCK(cn);
	}

	if ( fn )
	{
		FUNCTION_UNLOCK( fn );
	}

	if ( reg )
	{
		SEMAPHORE_RELEASE( & reg->reg_Semaphore );
	}

	return( next );
}

// --
