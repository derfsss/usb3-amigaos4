
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --
struct MsgPort *			USB3MsgPort				= NULL;
U32							USB3MsgPortBit			= 0;
struct IORequest *			USB3IOReq				= NULL;
struct Device *				USB3Base				= NULL;
struct USB3IFace *			IUSB3					= NULL;
// --
static struct MsgPort *		USBNotifyMsgPort		= NULL;
static PTR					USBHCDNotify			= NULL;
static PTR					USBFktNotify			= NULL;
static PTR					USBIfcNotify			= NULL;
U32							USBNotifyBit			= 0;
// --

static S32 Handle_Notify_HCD( struct USB3_NotifyMessage *msg );
static S32 Handle_Notify_Function( struct USB3_NotifyMessage *msg );
static S32 Handle_Notify_Interface( struct USB3_NotifyMessage *msg );
struct USB3_Descriptor *Desc_GetNext( struct USB3_Descriptor *desc );

// --

S32 USB_Setup( void )
{
S32 retval;

	retval = FALSE;

	USB3MsgPort = AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( ! USB3MsgPort )
	{
		DebugPrintF( "Error Creating USB Device MsgPort\n" );
		goto bailout;
	}

	USB3MsgPortBit = 1UL << USB3MsgPort->mp_SigBit;

	USB3IOReq = (PTR)AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_ReplyPort,	USB3MsgPort,
		ASOIOR_Size,		sizeof( struct IORequest ),
		TAG_END
	);

	if ( ! USB3IOReq )
	{
		DebugPrintF( "Error Creating USB Device IORequest\n" );
		goto bailout;
	}

	if ( OpenDevice( "usb3.device", 0, USB3IOReq, 0 ))
	{
		DebugPrintF( "Error Opening usb3.device\n" );
		goto bailout;
	}

	USB3Base = (PTR) USB3IOReq->io_Device;
	IUSB3 = (PTR) GetInterface( (PTR) USB3Base, "main", 1, NULL );

	if ( ! IUSB3 )
	{
		DebugPrintF( "Error Obtaining USB Interface\n" );
		goto bailout;
	}

	USBNotifyMsgPort = AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( ! USBNotifyMsgPort )
	{
		DebugPrintF( "Error allocating MsgPort" );
		goto bailout;
	}

	USBNotifyBit = 1UL << USBNotifyMsgPort->mp_SigBit;

	USBHCDNotify = USB3_Notify_Add( USBNT_HCD, USBNotifyMsgPort );

	if ( ! USBHCDNotify )
	{
		DebugPrintF( "Error setting up USB Notify" );
		goto bailout;
	}

	USBFktNotify = USB3_Notify_Add( USBNT_Function, USBNotifyMsgPort );

	if ( ! USBFktNotify )
	{
		DebugPrintF( "Error setting up USB Notify" );
		goto bailout;
	}

	USBIfcNotify = USB3_Notify_Add( USBNT_Interface, USBNotifyMsgPort );

	if ( ! USBIfcNotify )
	{
		DebugPrintF( "Error setting up USB Notify" );
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --

void USB_Cleanup( void )
{
PTR msg;

	if ( USBIfcNotify )
	{
		USB3_Notify_Remove( USBIfcNotify );
		USBIfcNotify = NULL;
	}

	if ( USBFktNotify )
	{
		USB3_Notify_Remove( USBFktNotify );
		USBFktNotify = NULL;
	}

	if ( USBHCDNotify )
	{
		USB3_Notify_Remove( USBHCDNotify );
		USBHCDNotify = NULL;
	}

	if ( USBNotifyMsgPort )
	{
		while(( msg = GetMsg( USBNotifyMsgPort )))
		{
			ReplyMsg( msg );
		}

		FreeSysObject( ASOT_PORT, USBNotifyMsgPort );
		USBNotifyMsgPort = NULL;
	}

	if ( IUSB3 )
	{
		DropInterface( (PTR) IUSB3 );
		IUSB3 = NULL;
	}

	if ( USB3Base )
	{
		CloseDevice( USB3IOReq );
		USB3Base = NULL;
	}

	if ( USB3IOReq )
	{
		FreeSysObject( ASOT_IOREQUEST, USB3IOReq );
		USB3IOReq = NULL;
	}

	if ( USB3MsgPort )
	{
		FreeSysObject( ASOT_PORT, USB3MsgPort );
		USB3MsgPort = NULL;
	}
}

// --

void Handle_Notify( void )
{
struct USB3_NotifyMessage *msg;
S32 rebuild;

	rebuild = FALSE;

	while(( msg = (PTR) GetMsg( USBNotifyMsgPort )))
	{
		switch( msg->nm_Type )
		{
			case USBNT_HCD:
			{
				rebuild |= Handle_Notify_HCD( msg );
				break;
			}

			case USBNT_Function:
			{
				rebuild |= Handle_Notify_Function( msg );
				break;
			}

			case USBNT_Interface:
			{
				rebuild |= Handle_Notify_Interface( msg );
				break;
			}

			default:
			{
				DebugPrintF( "Unknown Notify Type #%u\n", msg->nm_Type );
				break;
			}
		}

		ReplyMsg( & msg->nm_Message );
	}

	if ( rebuild )
	{
		Build_Device_Tree();
	}
}

// --

static STR HCDNames[HCDTYPE_Last] =
{
	[HCDTYPE_UHCI]		= "UHCI",	// PCI
	[HCDTYPE_OHCI]		= "OHCI",	// PCI
	[HCDTYPE_EHCI]		= "EHCI",	// PCI
	[HCDTYPE_XHCI]		= "XHCI",	// PCI
	[HCDTYPE_TABOR]		= "EHCI",	// Tabor Onboard EHCI controller (A1222)
	[HCDTYPE_P5020]		= "EHCI",	// X5020 Onboard EHCI Controller
	[HCDTYPE_P5040]		= "EHCI",	// X5040 Onboard EHCI Controller
	[HCDTYPE_SAM440]	= "OHCI",	// SAM440 Onboard OHCI controller
	[HCDTYPE_SAM460]	= "OHCI",	// SAM460 Onboard OHCI controller
};

static S32 Handle_Notify_HCD( struct USB3_NotifyMessage *msg )
{
struct USB3_FktNode *node;
STR name;
S32 rebuild;

	rebuild = FALSE;

	switch( msg->nm_Command )
	{
		case USBNC_Adding:
		{
			node = FktNode_Alloc( TYPE_HCD, msg->nm_ID );

			if ( ! node )
			{
				break;
			}

			if ( msg->nm_Data.nm_HCD.nm_Type < HCDTYPE_Last )
			{
				name = HCDNames[msg->nm_Data.nm_HCD.nm_Type];
			}
			else
			{
				name = NULL;
			}

			if ( name )
			{
				name = ASPrintf( "%s #%ld", name, msg->nm_Data.nm_HCD.nm_Index );
			}
			else
			{
				name = ASPrintf( "Unknown #%ld", msg->nm_Data.nm_HCD.nm_Index );
			}

			node->Name = name;

			// --

			AddTail( & DeviceTreeUSBList, (PTR) node );

			rebuild = TRUE;
			break;
		}

		case USBNC_Updating:
		{
			node = FktNode_Find( & DeviceTreeUSBList, msg->nm_ID );

			if ( node )
			{

				// There are nothing that can change atm

			}
			break;
		}

		case USBNC_Removing:
		{
			node = FktNode_Find( & DeviceTreeUSBList, msg->nm_ID );

			if ( node )
			{
				Remove( (PTR) node );

				FktNode_Delete( node );

				rebuild = TRUE;
			}
			break;
		}

		default:
		{
			DebugPrintF( "HCD : Unknown : %lx\n", msg->nm_ID );
			break;
		}
	}

	return(	rebuild );
}

// --

static S32 Handle_Notify_Function( struct USB3_NotifyMessage *msg )
{
struct USB3_HUB_Desc HUBDsc;
struct USB3_FktNode *parent;
struct USB3_FktNode *node;
S32 rebuild;
// U32 max;
// U32 cnt;
// U16 Buffer[128];

	rebuild = FALSE;

	parent = FktNode_Find( & DeviceTreeUSBList, msg->nm_Data.nm_Function.nm_Parent_ID );

	if ( ! parent )
	{
		DebugPrintF( "Parent Node not found\n" );
		goto bailout;
	}

	switch( msg->nm_Command )
	{
		case USBNC_Adding:
		{
			node = FktNode_Alloc( TYPE_FUNCTION, msg->nm_ID );

			if ( ! node )
			{
				DebugPrintF( "Error Allocating Memory" );
				break;
			}

			node->Port	= msg->nm_Data.nm_Function.nm_PortNr;

			// --

			USB3_Attr_Get( msg->nm_ID, USB3Tag_Attr_DeviceDesc, & node->DevDsc, sizeof( struct USB3_Device_Desc ));

			// --

			if ( node->DevDsc.DeviceClass == USBCLASS_HUB )
			{
				USB3_Attr_Get( msg->nm_ID, USB3Tag_Attr_HUBDesc, & HUBDsc, sizeof( struct USB3_HUB_Desc ));

				node->Ports = HUBDsc.NbrPorts;

				if ( node->Ports )
				{
					node->Functions = AllocVecTags( node->Ports * 4,
						AVT_Clear, 0,
						TAG_END
					);

					if ( ! node->Functions )
					{
						DebugPrintF( "Error Allocating Memory" );
						FktNode_Delete( node );
						break;
					}
				}
			}

			if ( parent->Functions )
			{
				parent->Functions[ node->Port-1 ] = node;
			}

			// --

			USB3_Attr_Get( msg->nm_ID, USB3Tag_Attr_ConfigSize, & node->CfgSize, sizeof( U32 ));
			// IUSB3->Stack_GetAttribute( msg->nm_ID, USB3Tag_FKT_ConfigDescriptorSize, &node->Data.Function.ConfigSize, sizeof( U32 ));

			node->CfgDesc = AllocVecTags( node->CfgSize + 2,	// Adding empty Type+Len
				AVT_Clear, 0,
				TAG_END
			);

			if ( ! node->CfgDesc )
			{
				FktNode_Delete( node );
				break;
			}

			USB3_Attr_Get( msg->nm_ID, USB3Tag_Attr_ConfigDesc, node->CfgDesc, node->CfgSize );
			// IUSB3->Stack_GetAttribute( msg->nm_ID, USB3Tag_FKT_ConfigDescriptor, node->Data.Function.ConfigDesc, node->Data.Function.ConfigSize );

			// --

			// Buffer[0] = 0;

			// IUSB3->Stack_GetAttribute( msg->nm_ID, USB3Tag_FKT_ManufacturerStr, &Buffer, 34 );

			// if ( Buffer[0] )
			// {
			// 	max = MIN(( Buffer[0] >> 9 ), 32 ) + 1;

			// 	node->Data.Function.ManufacturerStr[0] = '\"';

			// 	for( cnt=1 ; cnt<max ; cnt++ )
			// 	{
			// 		node->Data.Function.ManufacturerStr[cnt] = ( Buffer[cnt] ) >> 8;
			// 	}

			// 	node->Data.Function.ManufacturerStr[cnt++] = '\"';
			// 	node->Data.Function.ManufacturerStr[cnt] = 0;
			// }

			// --

			// Buffer[0] = 0;

			// IUSB3->Stack_GetAttribute( msg->nm_ID, USB3Tag_FKT_ProductStr, &Buffer, 256 );

			// if ( Buffer[0] )
			// {
			// 	max = MIN(( Buffer[0] >> 9 ), 32 ) + 1;

			// 	node->Data.Function.ProductStr[0] = '\"';

			// 	for( cnt=1 ; cnt<max ; cnt++ )
			// 	{
			// 		node->Data.Function.ProductStr[cnt] = ( Buffer[cnt] >> 8 );
			// 	}

			// 	node->Data.Function.ProductStr[cnt++] = '\"';
			// 	node->Data.Function.ProductStr[cnt] = 0;
			// }

			// --

			// Buffer[0] = 0;

			// IUSB3->Stack_GetAttribute( msg->nm_ID, USB3Tag_FKT_SerialStr, &Buffer, 256 );

			// if ( Buffer[0] )
			// {
			// 	max = MIN(( Buffer[0] >> 9 ), 32 ) + 1;

			// 	node->Data.Function.SerialStr[0] = '\"';

			// 	for( cnt=1 ; cnt<max ; cnt++ )
			// 	{
			// 		node->Data.Function.SerialStr[cnt] = ( Buffer[cnt] >> 8 );
			// 	}

			// 	node->Data.Function.SerialStr[cnt++] = '\"';
			// 	node->Data.Function.SerialStr[cnt] = 0;
			// }

			// --

			AddTail( & parent->Header, (PTR) node );

			rebuild = TRUE;
			break;
		}

		case USBNC_Updating:
		{
			node = FktNode_Find( & DeviceTreeUSBList, msg->nm_ID );

			if ( node )
			{
				if ( node->DevDsc.DeviceClass == USBCLASS_HUB )
				{
					USB3_Attr_Get( msg->nm_ID, USB3Tag_Attr_HUBDesc, & HUBDsc, sizeof( struct USB3_HUB_Desc ));

					if (( node->Ports == 0 ) && ( node->Ports != HUBDsc.NbrPorts ))
					{
						node->Ports = HUBDsc.NbrPorts;

						node->Functions = AllocVecTags( node->Ports * 4,
							AVT_Clear, 0,
							TAG_END
						);

						rebuild = TRUE;
					}
				}
			}
			break;
		}

		case USBNC_Removing:
		{
			node = FktNode_Find( & DeviceTreeUSBList, msg->nm_ID );

			if (( node ) && ( parent->Functions ))
			{
				parent->Functions[ node->Port-1 ] = NULL;
			}

			if ( node )
			{
				FktNode_Reset( node );

				rebuild = TRUE;
			}
			break;
		}

		default:
		{
			break;
		}
	}

bailout:

	return(	rebuild );
}

// --

static S32 Handle_Notify_Interface( struct USB3_NotifyMessage *msg )
{
struct USB3_Descriptor *dsc;
struct USB3_FktNode *parent;
struct USB3_FktNode *node;
U32 cfgnr;
// U32 cnt;
// U32 max;
// U16 Buffer[128];
S32 len;
S32 rebuild;

	rebuild = FALSE;

	parent = FktNode_Find( & DeviceTreeUSBList, msg->nm_Data.nm_Interface.nm_Function_ID );

	if ( ! parent )
	{
		DebugPrintF( "Parent Node not found\n" );
		goto bailout;
	}

	switch( msg->nm_Command )
	{
		case USBNC_Adding:
		{
			node = FktNode_Alloc( TYPE_INTERFACE, msg->nm_ID );

			if ( ! node )
			{
				break;
			}

			// --

			dsc = (PTR) parent->CfgDesc;
			len = (S32) parent->CfgSize;

			if ( ! node->CfgDesc )
			{
				break;
			}

			node->IfcDesc = NULL;

			cfgnr = -1;

			while(( dsc ) && ( len > 0 ) && ( ! node->IfcDesc ))
			{
				/**/ if ( dsc->DescriptorType == DSCTYPE_Configuration )
				{
					struct USB3_Config_Desc * cfgdsc = (PTR) dsc ;

					cfgnr = cfgdsc->ConfigurationValue;
				}
				else if ( dsc->DescriptorType == DSCTYPE_Interface )
				{
					struct USB3_Interface_Desc * ifcdsc = (PTR) dsc ;

					while( TRUE )
					{
						if ( msg->nm_Data.nm_Interface.nm_Config_Nr	!= cfgnr )
						{
							break;
						}

						if ( msg->nm_Data.nm_Interface.nm_Interface_Nr != ifcdsc->InterfaceNumber )
						{
							break;
						}

						if ( msg->nm_Data.nm_Interface.nm_Interface_AltNr != ifcdsc->AlternateSetting )
						{
							break;
						}

						node->IfcDesc = ifcdsc;
						node->IfcSize = len;
						break;
					}
				}

				len -= dsc->Length;
				dsc  = Desc_GetNext( dsc );
			}

			if ( ! node->IfcDesc )
			{
				break;
			}

			// --

			// DebugPrintF( "USBNC_Adding 3" );
			// U32 ii = msg->nm_Data.nm_Interface.nm_Interface_StringID;

			// if ( ii )
			// {
			// 	DebugPrintF( "String IDnr: %ld", ii );

			// 	Buffer[0] = 0;

			// 	DebugPrintF( "USBNC_Adding 3.4" );

			// 	IUSB3->Stack_GetAttribute( msg->nm_Data.nm_Interface.nm_Interface_StringID, USB3Tag_IFC_String, & Buffer, sizeof( Buffer ));

			// 	DebugPrintF( "USBNC_Adding 3.5" );

			// 	if ( Buffer[0] )
			// 	{
			// 		max = MIN(( Buffer[0] >> 9 ), 32 ) + 1;

			// 		DebugPrintF( "USBNC_Adding 3.6" );

			// 		node->Data.Interface.InterfaceStr[0] = '\"';

			// 		for( cnt=1 ; cnt<max ; cnt++ )
			// 		{
			// 			node->Data.Interface.InterfaceStr[cnt] = ( Buffer[cnt] >> 8 );
			// 		}

			// 		DebugPrintF( "USBNC_Adding 3.7" );

			// 		node->Data.Interface.InterfaceStr[cnt++] = '\"';
			// 		node->Data.Interface.InterfaceStr[cnt] = 0;
			// 	}
			// }

			// --

			AddTail( & parent->Header, (PTR) node );

			rebuild = TRUE;
			break;
		}

		case USBNC_Updating:
		{
			node = FktNode_Find( & DeviceTreeUSBList, msg->nm_ID );

			if ( node )
			{

				// There are nothing that can change atm

			}
			break;
		}

		case USBNC_Removing:
		{
			node = FktNode_Find( & DeviceTreeUSBList, msg->nm_ID );

			if ( node )
			{
				Remove( (PTR) node );

				FktNode_Delete( node );

				rebuild = TRUE;
			}
			break;
		}

		default:
		{
			break;
		}
	}

bailout:

	return(	rebuild );
}

// --

STR GetClassName( U32 class, U32 subclass, U32 protocol )
{
STR name;

	switch( class )
	{
		case USBCLASS_Audio:
		{
			switch( subclass )
			{
				case 0x01:
				{
					name = "Audio Control";
					break;
				}

				case 0x02:
				{
					name = "Audio Streaming";
					break;
				}

				case 0x03:
				{
					name = "Audio MIDI Streaming";
					break;
				}

				default:
				{
					name = "Audio";
					break;
				}
			}
			break;
		}

		case USBCLASS_CDC_Control:
		{
			name = "Communications and CDC Control";
			break;
		}

		case USBCLASS_HID:
		{
			name = "HID Device";

			if ( subclass == 1 ) // Boot Protocol mode
			{
				switch( protocol )
				{
					case 1:
					{
						name = "HID Keyboard";
						break;
					}

					case 2:
					{
						name = "HID Mouse";
						break;
					}

					default:
					{
						break;
					}
				}
			}

			break;
		}

		case USBCLASS_Physical:
		{
			name = "Physical";
			break;
		}

		case USBCLASS_Image:
		{
			name = "Image";
			break;
		}

		case USBCLASS_Printer:
		{
			switch( protocol )
			{
				case 0x01:
				{
					name = "Unidirectional Printer";
					break;
				}

				case 0x02:
				{
					name = "Bidirectional Printer";
					break;
				}

				case 0x03:
				{
					name = "IEEE1284.4 Printer (Dot4)";
					break;
				}

				case 0x04:
				{
					name = "Vendor Specific Printer";
					break;
				}

				default:
				{
					name = "Printer";
					break;
				}
			}
			break;
		}

		case USBCLASS_Mass_Storage:
		{
			switch( protocol )
			{
				default:
				{
					name = "Mass Storage";
					break;
				}
			}
			break;
		}

		case USBCLASS_HUB:
		{
			name = "Hub";
			break;
		}

		case USBCLASS_CDC_Data:
		{
			name = "CDC Data";
			break;
		}

		case USBCLASS_Smart_Card:
		{
			name = "Smart Card";
			break;
		}

		case USBCLASS_Content_Security:
		{
			name = "Content Security";
			break;
		}

		case USBCLASS_Video:
		{
			switch( subclass )
			{
				case 0x01:
				{
					name = "Video Control";
					break;
				}

				case 0x02:
				{
					name = "Video Streaming";
					break;
				}

				case 0x03:
				{
					name = "Video Collection";
					break;
				}

				default:
				{
					name = "Video";
					break;
				}
			}
			break;
		}

		case USBCLASS_Personal_Healthcase:
		{
			name = "Personal Healthcare";
			break;
		}

		case USBCLASS_Diagnostic_Device:
		{
			name = "Diagnostic Device";
			break;
		}

		case USBCLASS_Wireless_Controller:
		{
			name = "Wireless Controller";
			break;
		}

		case USBCLASS_Miscellaneous:
		{
			name = "Miscellaneous";
			break;
		}

		case USBCLASS_Application_Specific:
		{
			name = "Application";
			break;
		}

		default:
		case USBCLASS_Vendor_Specific:
		{
			name = "Vendor Specific";
			break;
		}
	}

	return( name );
}

// --

struct USB3_Descriptor *Desc_GetNext( struct USB3_Descriptor *desc )
{
struct USB3_Descriptor *dsc;
U8 *data;
U32 pos;

	dsc = NULL;

	if ( ! desc )
	{
		goto bailout;
	}

	if (( desc->Length ) && ( desc->DescriptorType ))
	{
		data = (PTR) desc;
		pos = desc->Length;

		desc = (PTR) & data[pos];

		if (( desc->Length ) && ( desc->DescriptorType ))
		{
			dsc = desc;
		}
	}

bailout:

	return( dsc );
}

// --
