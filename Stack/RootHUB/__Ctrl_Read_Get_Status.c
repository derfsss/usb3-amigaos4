
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Get_Status( 
		struct USB3_SetupData *sd, 
UNUSED	struct USB3_HCDNode *hn, 
UNUSED	struct USBBase *usbbase,
		PTR buffer, 
		U32 *max )
{
S32 err;
U16 val;

	/**/ if	( sd->RequestType == ( REQTYPE_Read | REQTYPE_Standard | REQTYPE_Device ))
	{
		struct USB3_DeviceStatus *devstat = buffer;

		// Get Device Status

		USBDEBUG( "RootHUB : CTRL Read : Get Device Status" );

		val  = USBSTATUS_Device_SelfPowered;
//		val |= USBSTATUS_Device_RemoteWakeup;
		devstat->Stat = LE_SWAP16( val );

		*max = sizeof( struct USB3_DeviceStatus );

		err = USB3Err_NoError;
	}
	else if	( sd->RequestType == ( REQTYPE_Read | REQTYPE_Standard | REQTYPE_Interface ))
	{
		struct USB3_InterfaceStatus *ifcstat = buffer;

		// Get Interface Status

		USBDEBUG( "RootHUB : CTRL Read : Get Interface Status" );

		val = 0;
		ifcstat->Stat = LE_SWAP16( val );

		*max = sizeof( struct USB3_InterfaceStatus );

		err = USB3Err_NoError;
	}
	else if	( sd->RequestType == ( REQTYPE_Read | REQTYPE_Standard | REQTYPE_EndPoint ))
	{
		struct USB3_EndPointStatus *epstat = buffer;

		// Get EndPoint Status

		USBDEBUG( "RootHUB : CTRL Read : Get EndPoint Status" );

		val = 0;
		epstat->Stat = LE_SWAP16( val );

		*max = sizeof( struct USB3_EndPointStatus );

		err = USB3Err_NoError;
	}
	else if	( sd->RequestType == ( REQTYPE_Read | REQTYPE_Class | REQTYPE_Device ))
	{
		struct USB3_HUBStatus *hubstat = buffer;

		// Get HUB Status

		USBDEBUG( "RootHUB : CTRL Read : Get HUB Status" );

		// --

		val = 0;
		hubstat->wHubStatus = LE_SWAP16( val );

//		#define HUBSTATF_Local_Power			0x01
//		#define HUBSTATF_Over_Current			0x02

		// --

		val = 0;
		hubstat->wHubChange = LE_SWAP16( val );

//		#define HUBCHGF_Local_Power_Chg		0x01
//		#define HUBCHGF_Over_Current_Chg		0x02

		// --

		*max = sizeof( struct USB3_HUBStatus );

		err = USB3Err_NoError;
	}
	else if ( sd->RequestType == ( REQTYPE_Read | REQTYPE_Class | REQTYPE_Other ))
	{
		struct USB3_PortStatus *portstat = buffer;
		U32 status;
		U16 wIndex;
		U32 port;
		
		// Get Port Status
		
		// The HCD's return HUBF_ flags, we here converts them into HUB Port flags
		// So the RootHub understands the Flags

		wIndex = LE_SWAP16( sd->Index );
		port = ( wIndex & 0x00ff );

		// Get HCD Driver Port Status
		if ( hn->HCD_Functions.Port_Get_Status )
		{
			USBDEBUG( "RootHUB : CTRL Read : PORT_GET_STATUS : Port #%ld", port );

			status = hn->HCD_Functions.Port_Get_Status( hn, port );
		}
		else
		{
			USBERROR( "RootHUB : CTRL Read : PORT_GET_STATUS : Not Implemented in HCD" );

			status = 0;
		}

		val = 	(( status &	HUBF_Status_Connection )	? PORTSTATF_Connection	: 0 ) |
				(( status &	HUBF_Status_Enable )		? PORTSTATF_Enable		: 0 ) |
				(( status &	HUBF_Status_Suspend )		? PORTSTATF_Suspend		: 0 ) |
				(( status &	HUBF_Status_Over_Current )	? PORTSTATF_Over_Current: 0 ) |
				(( status &	HUBF_Status_Reset )			? PORTSTATF_Reset		: 0 ) |
				(( status &	HUBF_Status_Power )			? PORTSTATF_Power		: 0 ) |
				(( status &	HUBF_Status_Low_Speed )		? PORTSTATF_Low_Speed	: 0 ) |
				(( status &	HUBF_Status_High_Speed )	? PORTSTATF_High_Speed	: 0 ) |
				(( status &	HUBF_Status_Test )			? PORTSTATF_Test		: 0 ) |
				(( status &	HUBF_Status_Indicator )		? PORTSTATF_Indicator	: 0 ) ;

		portstat->wPortStatus = LE_SWAP16( val );

		val =	(( status &	HUBF_Chg_Connection )	? PORTCHGF_Connection	: 0 ) |
				(( status &	HUBF_Chg_Enable )		? PORTCHGF_Enable		: 0 ) |
				(( status &	HUBF_Chg_Suspend )		? PORTCHGF_Suspend		: 0 ) |
				(( status &	HUBF_Chg_Over_Current )	? PORTCHGF_Over_Current : 0 ) |
				(( status &	HUBF_Chg_Reset )			? PORTCHGF_Reset		: 0 ) ;

		portstat->wPortChange = LE_SWAP16( val );

		*max = sizeof( struct USB3_PortStatus );

		err = USB3Err_NoError;
	}
	else
	{
		USBDEBUG( "RootHUB : CTRL Read : Unknown Request Type $%04lx", (U32) sd->RequestType );
	}

	return( err );
}

// --
