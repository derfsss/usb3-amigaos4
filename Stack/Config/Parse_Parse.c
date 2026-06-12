
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static struct USB2_InterfaceHeader *Find_InterfaceHeader(
	struct USBBase *usbbase,
	struct USB2_ConfigNode *cn,
	struct USB2_Interface_Desc *ifcdsc )
{
struct USB2_InterfaceHeader *ih;
struct USB2_InterfaceGroup *ig;
U32 nr;

	// --

	ih = NULL;
	ig = cn->cfg_InterfaceGroups.uh_Head;
	nr = ifcdsc->InterfaceNumber;

	while( INTERFACE_VALIDGROUP(ig) == VSTAT_Okay )
	{
		// We are using the fact that each IG only have one IH at this point
		ih = ig->ig_Group.uh_Head;

		if ( ih->ih_Number == nr )
		{
			break;
		}
		else
		{
			ig = Node_Next( ig );
			ih = NULL;
		}
	}

	// --

	if ( INTERFACE_VALIDGROUP(ig) != VSTAT_Okay )
	{
		ig = INTERFACE_ALLOCGROUP(cn);
		ih = INTERFACE_ALLOCHEADER( ig, nr );

		if ((ig) && (ih))
		{
			ig->ig_Class	= ifcdsc->InterfaceClass;
			ig->ig_SubClass	= ifcdsc->InterfaceSubClass;
			ig->ig_Protocol	= ifcdsc->InterfaceProtocol;

			NODE_ADDTAIL( & cn->cfg_InterfaceGroups, ig );
			NODE_ADDTAIL( & ig->ig_Group, ih );
		}
		else
		{
			INTERFACE_FREEHEADER(ih);
			INTERFACE_FREEGROUP(ig);
			ih = NULL;
		}
	}

	// --

	return( ih );
}

// --

// --
// Parse Config Descriptor and build internal Structions

SEC_CODE static S32 __Parse( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
struct USB2_InterfaceHeader *ih;
struct USB2_Interface_Desc *ifcdsc;
struct USB2_EndPoint_Desc *epdsc;
struct USB2_InterfaceNode *in;
struct USB2_EndPointNode *ep;
struct USB2_Config_Desc *cfgdsc;
struct USB2_Descriptor *dsc;
struct USB2_ConfigNode *cn;
S32 retval;
S32 len;

	retval	= FALSE;
	dsc		= fn->fkt_Config_Desc_Buf;
	len		= fn->fkt_Config_Desc_Size;
	cn		= NULL;
	in		= NULL;

	while(( dsc ) && ( len > 1 ))
	{
		switch( dsc->DescriptorType )
		{
			case DSCTYPE_Configuration:
			{
				USBDEBUG( "Config Descriptor" );

				in = NULL;
				cn = CONFIG_ALLOC(fn);

				if ( ! cn )
				{
					USBERROR( "__Parse: Error allocating memory" );
					goto bailout;
				}

				cfgdsc = (PTR) dsc;
				cn->cfg_Descriptor = cfgdsc;
				cn->cfg_Number = cfgdsc->ConfigurationValue;

				NODE_ADDTAIL( & fn->fkt_Configs, cn );
				break;
			}

			/*
			** FN : 0x6FE5DB50, Class: 0000, SubClass: 0000, Protocol: 0000, Addr: 3, Tier: 2
			** CN : 0x6FF0B850, ConfigNr 1
			**  IG : 0x6F9690A0, Class: 0003, SubClass: 0001, Protocol: 0001
			**   IH : 0x6FF0B8D0, InterfaceNr: 0
			**    IN : 0x6FF0AE70, AltNr: 0
			**     EP : 0x6F95D0C0, Type: 3, Num: 1
			**  IG : 0x6F9690D0, Class: 0003, SubClass: 0000, Protocol: 0000
			**   IH : 0x6FF0B890, InterfaceNr: 1
			**    IN : 0x6F95D160, AltNr: 0
			**     EP : 0x6F95D1B0, Type: 3, Num: 2
			*/

			case DSCTYPE_Interface:
			{
				USBDEBUG( "Interface Descriptor" );

				if ( CONFIG_VALID(cn) != VSTAT_Okay )
				{
					USBERROR( "__Parse: Error Unexpected Interface" );
					goto bailout;
				}

				ifcdsc = (PTR) dsc;

				ih = Find_InterfaceHeader( usbbase, cn, ifcdsc );

				if ( INTERFACE_VALIDHEADER(ih) != VSTAT_Okay )
				{
					USBERROR( "__Parse: Error allocating memory" );
					goto bailout;
				}

				in = INTERFACE_ALLOCNODE(ih);

				if ( ! in )
				{
					USBERROR( "__Parse: Error allocating memory" );
					goto bailout;
				}

				in->in_Descriptor	= ifcdsc;
				in->in_AltNumber	= ifcdsc->AlternateSetting;

				if ( ! ih->ih_Active )
				{
					ih->ih_Active		= in;
					ih->ih_AltNumber	= in->in_AltNumber;
				}

				NODE_ADDTAIL( & ih->ih_AltSettings, in );
				break;
			}

			case DSCTYPE_EndPoint:
			{
				USBDEBUG( "EndPoint Descriptor" );

				if ( INTERFACE_VALIDNODE(in) != VSTAT_Okay )
				{
					USBERROR( "__Parse: Error Unexpected EndPoint" );
					goto bailout;
				}

				epdsc = (PTR) dsc;

				ep = ENDPOINT_ALLOC( fn, in, epdsc );

				if ( ! ep )
				{
					USBERROR( "__Parse: Error allocating memory" );
					goto bailout;
				}

				NODE_ADDTAIL( & in->in_EndPoints, ep );
				break;
			}

			default:
			{
				break;
			}
		}

		len -= dsc->Length;
		dsc  = DESC_NEXT_DESC( dsc );
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --
