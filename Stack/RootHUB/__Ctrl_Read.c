
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

static const struct USB2_Device_Desc HUB11_DeviceDsc =
{
/* Length			*/ sizeof( struct USB2_Device_Desc ),
/* DescriptorType	*/ DSCTYPE_Device,
/* USB				*/ LE_SWAP16( 0x0110 ),
/* DeviceClass		*/ USBCLASS_HUB,
/* DeviceSubClass	*/ 0x00,
/* DeviceProtocol	*/ 0x00,
/* MaxPacketSize0	*/ 8,
/* VendorID			*/ LE_SWAP16( 0x0000 ),
/* DeviceID			*/ LE_SWAP16( 0x0000 ),
/* DeviceID			*/ LE_SWAP16( 0x0000 ),
/* ManufacturerStrNr*/ 0x00,
/* DeviceStrNr		*/ 0x00,
/* SerialStrNr		*/ 0x00,
/* NumConfigurations*/ 1,
};

static const struct USB2_Device_Desc HUB20_DeviceDsc =
{
/* Length			*/ sizeof( struct USB2_Device_Desc ),
/* DescriptorType	*/ DSCTYPE_Device,
/* USB				*/ LE_SWAP16( 0x0200 ),
/* DeviceClass		*/ USBCLASS_HUB,
/* DeviceSubClass	*/ 0x00,
/* DeviceProtocol	*/ 0x00,
/* MaxPacketSize0	*/ 64,
/* VendorID			*/ LE_SWAP16( 0x0000 ),
/* DeviceID			*/ LE_SWAP16( 0x0000 ),
/* DeviceID			*/ LE_SWAP16( 0x0000 ),
/* ManufacturerStrNr*/ 0x00,
/* DeviceStrNr		*/ 0x00,
/* SerialStrNr		*/ 0x00,
/* NumConfigurations*/ 1,
};

static const struct myHUB_Cfg HUBConfigDsc =
{
{ // struct USB2_Config_Desc
/* Length			*/ sizeof( struct USB2_Config_Desc ),
/* DescriptorType	*/ DSCTYPE_Configuration,
/* TotalLength		*/ LE_SWAP16( sizeof( struct myHUB_Cfg ) ),
/* NumInterfaces	*/ 0x00,
/* ConfigurationValu*/ HUB_Cfg_Nr,
/* Configuration	*/ 0x00,
/* Attributes		*/ 0x00,
/* MaxPower			*/ 0x00,
},
{ // struct USB2_Interface_Desc
/* Length			*/ sizeof( struct USB2_Interface_Desc ),
/* DescriptorType	*/ DSCTYPE_Interface,
/* InterfaceNumber	*/ HUB_Ifc_Nr,
/* AlternateSetting	*/ 0x00,
/* NumEndPoints		*/ 1,
/* InterfaceClass	*/ 0x00,
/* InterfaceSubClass*/ 0x00,
/* InterfaceProtocol*/ 0x00,
/* InterfaceStrNr	*/ 0x00,
},
{ // struct USB2_EndPoint_Desc
/* Length			*/ sizeof( struct USB2_EndPoint_Desc ),
/* DescriptorType	*/ DSCTYPE_EndPoint,
/* EndPointAddress	*/ EPADR_Dir_Number( HUB_IntEp_Nr ) | EPADR_Dir_In,
/* Attributes		*/ EPATT_Type_Interrupt,
/* MaxPacketSize	*/ LE_SWAP16( 8 ),
/* Interval			*/ 12,
}
};

static const struct USB2_String_Desc LangIDs =
{
/* Length			*/ 4,
/* DescType			*/ DSCTYPE_String,
{
/* LangID			*/ LE_SWAP16( 0x0409 ),		// English (United States)
},
};

// --

#include "__Ctrl_Read_Get_Config.c"
#include "__Ctrl_Read_Get_Descriptor.c"
#include "__Ctrl_Read_Get_Interface.c"
#include "__Ctrl_Read_Get_Status.c"

// --

#define MAX_BUFFER	512

SEC_CODE static void __Control_Read( struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB2_SetupData *sd;
U32 max;
U32 len;
S32 err;
U8 buf[MAX_BUFFER];

	sd = ioreq->req_Public.io_SetupData;

	#ifdef DO_DEBUG
	MEM_SET( buf, 0x44, MAX_BUFFER );
	#endif

	if ( ! sd )
	{
		USBDEBUG( "RootHUB : Read : No SetupData buffer given" );

		err = USB2Err_Host_Stall;
	}
	else
	{
		max	= 0;

		switch( sd->RequestCode )
		{
			case REQCODE_Get_Status:
			{
				USBDEBUG( "RootHUB : Read : Get Status" );

				err = __Get_Status( sd, hn, usbbase, & buf, & max );
				break;
			}

			case REQCODE_Get_Descriptor:
			{
				USBDEBUG( "RootHUB : Read : Get Descriptor" );

				err = __Get_Descriptor( sd, hn, usbbase, & buf, & max );
				break;
			}

			case REQCODE_Get_Configuration:
			{
				USBDEBUG( "RootHUB : Read : Get Configuration" );

				err = __Get_Config( sd, hn, usbbase, & buf, & max );
				break;
			}

			case REQCODE_Get_Interface:
			{
				USBDEBUG( "RootHUB : Read : Get Interface" );

				err = __Get_Interface( sd, hn, usbbase, & buf, & max );
				break;
			}

			default:
			{
				USBDEBUG( "RootHUB : Read : Unknown Request (%lu)", (U32) sd->RequestCode );

				err = USB2Err_Host_Stall;
				break;
			}
		}

		#ifdef DEVBUG

		if ( max > MAX_BUFFER )
		{
			USBPANIC( "RootHUB : Read : DO_ERROR Buffer overflow (Max %lu, )" );
		}

		#endif

		if ( err == USB2Err_NoError )
		{
			len = MIN( max, ioreq->req_Public.io_Length );

			if ( len )
			{
				MEM_COPY( buf, ioreq->req_Public.io_Data, len );
			}
			else
			{
				USBERROR( "Error io_Actual is Zero (max %ld, io_Length %ld)", max, ioreq->req_Public.io_Length );
			}

			ioreq->req_Public.io_Actual = len;
		}
	}

	ioreq->req_Public.io_Error = err;
}

// --
