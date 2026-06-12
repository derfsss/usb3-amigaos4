
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Get_Descriptor( 
	struct USB3_SetupData *sd, 
	struct USB3_HCDNode *hn, 
	struct USBBase *usbbase,
	PTR buffer,
	U32 *max )
{
U16 wValue;
U16 dsctype;
U16 dscnr;
S32 err;

	wValue	= LE_SWAP16( sd->Value );
	dsctype = ( wValue & 0xff00 ) >> 8;
	dscnr	= ( wValue & 0x00ff ) >> 0;
	err		= USB3Err_Host_Stall;

	switch( dsctype )
	{
		case DSCTYPE_Device:
		{
			USBDEBUG( "Device Descriptor" );

			if ( hn->hn_HCDType == HCDTYPE_EHCI )
			{
				MEM_COPY( (PTR) & HUB20_DeviceDsc, buffer, sizeof( struct USB3_Device_Desc ));
			}
			else
			{
				MEM_COPY( (PTR) & HUB11_DeviceDsc, buffer, sizeof( struct USB3_Device_Desc ));
			}

			*max = sizeof( struct USB3_Device_Desc );

			err = USB3Err_NoError;
			break;
		}

		case DSCTYPE_Configuration:
		{
			USBDEBUG( "Configuration Descriptor" );

			MEM_COPY( (PTR) & HUBConfigDsc, buffer, sizeof( struct myHUB_Cfg ));

			*max = sizeof( struct myHUB_Cfg );

			err = USB3Err_NoError;
			break;
		}

		case DSCTYPE_String:
		{
			USBDEBUG( "String Descriptor" );

			switch( dscnr )
			{
				case 0:
				{
					MEM_COPY( (PTR) & LangIDs, buffer, sizeof( LangIDs ));

					*max = sizeof( LangIDs );

					err = USB3Err_NoError;
					break;
				}

				default:
				{
					break;
				}
			}
			break;
		}

		case DSCTYPE_Interface:
		{
			USBDEBUG( "Interface Descriptor" );
			break;
		}

		case DSCTYPE_EndPoint:
		{
			USBDEBUG( "EndPoint Descriptor" );
			break;
		}

		case DSCTYPE_Device_Qualifier:
		{
			USBDEBUG( "Device Qualifier Descriptor" );
			break;
		}

		case DSCTYPE_Other_Speed_Configuration:
		{
			USBDEBUG( "Other Speec Config Descriptor" );
			break;
		}

		case DSCTYPE_Interface_Power:
		{
			USBDEBUG( "Interface Power Descriptor" );
			break;
		}

		case 0x29: // HUB Desc
		{
			struct USB3_HUB_Desc *HUBDesc;
			U32 len;

			USBDEBUG( "Get HUB Descriptor" );

			HUBDesc = buffer;

			len = 7 + (( hn->hn_HUB_NumPorts + 7 ) / 8 );

			HUBDesc->Length				= len;
			HUBDesc->DescriptorType		= 0x29;
			HUBDesc->NbrPorts			= hn->hn_HUB_NumPorts;
			HUBDesc->HubCharacteristics = LE_SWAP16( 0 );
			HUBDesc->PwrOn2PwrGood		= 0;
			HUBDesc->HubContrCurrent	= 0;

			if ( len > 7 )
			{
				MEM_SET( & HUBDesc->Data, 0, len - 7 );
			}

			*max = len;

			err = USB3Err_NoError;
			break;
		}

		default:
		{
			USBDEBUG( "Unknown Descriptor Type (%ld)", dsctype );
			break;
		}
	}

	return( err );
}

// --
