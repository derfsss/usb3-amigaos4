
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Merge( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
struct USB3_Association_Desc *adsc;
struct USB3_InterfaceHeader *ih;
struct USB3_InterfaceGroup *ig2;
struct USB3_InterfaceGroup *ig;
struct USB3_ConfigNode *cfgnode;
struct USB3_Descriptor *dsc;
struct USB3_ConfigNode *cn;
S32 retval;
S32 len;
S32 cnt;

	retval = FALSE;

	if (( fn->fkt_Class	== 0xef )
	&&	( fn->fkt_SubClass == 0x02 )
	&&	( fn->fkt_Protocol	== 0x01 ))
	{
		USBDEBUG( "Merge Groups" );

		dsc		= fn->fkt_Config_Desc_Buf;
		len		= fn->fkt_Config_Desc_Size;
		cfgnode	= NULL;

		while(( dsc ) && ( len > 1 ))
		{
			switch( dsc->DescriptorType )
			{
				case DSCTYPE_Configuration:
				{
					cn = fn->fkt_Configs.uh_Head;

					while( CONFIG_VALID(cn) == VSTAT_Okay )
					{
						if ( cn->cfg_Number == ((struct USB3_Config_Desc *)dsc)->ConfigurationValue )
						{
							break;
						}
						else
						{
							cn = Node_Next( cn );
						}
					}

					cfgnode = cn;
					break;
				}

				case DSCTYPE_Interface_Association:
				{
					if ( CONFIG_VALID(cfgnode) != VSTAT_Okay )
					{
						USBDEBUG( "NULL Pointer" );
						break;
					}

					adsc = (PTR) dsc;

					ig = cfgnode->cfg_InterfaceGroups.uh_Head;

					while( INTERFACE_VALIDGROUP(ig) == VSTAT_Okay )
					{
						ih = ig->ig_Group.uh_Head;

						if ( ih->ih_Number == adsc->FirstInterface )
						{
							break;
						}
						else
						{
							ig = Node_Next( ig );
						}
					}

					if ( INTERFACE_VALIDGROUP(ig) != VSTAT_Okay )
					{
						USBDEBUG( "Inferface number #%ld not found", adsc->FirstInterface );
						break;
					}

					ig->ig_Class	= adsc->FunctionClass;
					ig->ig_SubClass	= adsc->FunctionSubClass;
					ig->ig_Protocol	= adsc->FunctionProtocol;

					for( cnt=1 ; cnt<adsc->InterfaceCount ; cnt++ )
					{
						ig2 = Node_Next( ig );

						if ( INTERFACE_VALIDGROUP(ig2) != VSTAT_Okay )
						{
							USBDEBUG( "Unexpected NULL Pointer" );
							break;
						}

						NODE_REMNODE( & cfgnode->cfg_InterfaceGroups, ig2 );

						while(( ih = NODE_REMHEAD( & ig2->ig_Group )))
						{
							INTERFACE_UNLOCKGROUP( ih->ih_Group );
							ih->ih_Group = ig;
							INTERFACE_LOCKGROUP( ih->ih_Group );

							NODE_ADDTAIL( & ig->ig_Group, ih );
						}
						
						INTERFACE_FREEGROUP( ig2 );
					}
					break;
				}
			}

			len -= dsc->Length;
			dsc  = DESC_NEXT_DESC( dsc );
		}

		USBDEBUG( "Merge Groups Done" );
	}

	retval = TRUE;

//bailout:

	return( retval );
}

// --
