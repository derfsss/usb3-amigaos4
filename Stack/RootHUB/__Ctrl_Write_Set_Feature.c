
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Set_Feature( 
		struct USB2_SetupData *sd, 
UNUSED	struct USB2_HCDNode *hn,
UNUSED	struct USBBase *usbbase )
{
//U16 wLength;
//U16 wValue;
U16 wIndex;
U16 port;
S32 err;

	err		= USB2Err_Host_Stall;
//	wValue	= LE_SWAP16( sd->Value );
	wIndex	= LE_SWAP16( sd->Index );
//	wLength	= LE_SWAP16( sd->Length );

	/**/ if	( sd->RequestType == ( REQTYPE_Write | REQTYPE_Class | REQTYPE_Device ))
	{
		// Set HUB Status

		USBDEBUG( "Set HUB Staus??" );

		// C_HUB_LOCAL_POWER = 0,
		// C_HUB_OVER_CURRENT,

		#if 0
		switch( LE_SWAP16( sd->Value ))
		{
			default:
			{
				break;
			}
		}
		#endif
	}
	else if ( sd->RequestType == ( REQTYPE_Write | REQTYPE_Class | REQTYPE_Other ))
	{
		// Set Port Status

		// PORT_CONNECTION = 0,
		// PORT_ENABLE,
		// PORT_SUSPEND,
		// PORT_OVER_CURRENT,
		// PORT_RESET,
		
		// PORT_POWER = 8,
		// PORT_LOW_SPEED,
		// PORT_HIGH_SPEED,
		//
		// PORT_CLR_CONNECTION = 16,
		// PORT_CLR_ENABLE,
		// PORT_CLR_SUSPEND,
		// PORT_CLR_OVER_CURRNET,
		// PORT_CLR_RESET,
		//
		// PORT_TEST,
		// PORT_INDICATOR,

		port = ( wIndex & 0x00ff );

		USBDEBUG( "SetPortFeature on port %lu", port );

		switch( LE_SWAP16( sd->Value ))
		{
			case PORT_ENABLE:
			{
				if ( hn->HCD_Functions.Port_Set_Enable )
				{
					USBDEBUG( "__Set_Feature : PORT_ENABLE : Port #%ld", port );

					err = hn->HCD_Functions.Port_Set_Enable( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Set_Feature : PORT_ENABLE : Not Implemented in HCD" );
				}
				#endif
				break;
			}

			case PORT_SUSPEND:
			{
				if ( hn->HCD_Functions.Port_Set_Suspend )
				{
					USBDEBUG( "__Set_Feature : PORT_SUSPEND : Port #%ld", port );

					err = hn->HCD_Functions.Port_Set_Suspend( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Set_Feature : PORT_SUSPEND : Not Implemented in HCD" );
				}
				#endif
				break;
			}

			case PORT_RESET:
			{
				if ( hn->HCD_Functions.Port_Set_Reset )
				{
					USBDEBUG( "__Set_Feature : PORT_RESET : Port #%ld", port );

					err = hn->HCD_Functions.Port_Set_Reset( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Set_Feature : PORT_RESET : Not Implemented in HCD" );
				}
				#endif
				break;
			}

			case PORT_POWER:
			{
				if ( hn->HCD_Functions.Port_Set_Power )
				{
					USBDEBUG( "__Set_Feature : PORT_POWER : Port #%ld", port );

					err = hn->HCD_Functions.Port_Set_Power( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Set_Feature : PORT_POWER : Not Implemented in HCD" );
				}
				#endif
				break;
			}

			#if 0
			case PORT_TEST:
			{
				err = HCDFunctions->Port_Set_Test( hn, port );
				break;
			}

			case PORT_INDICATOR:
			{
				err = PORT_SET_INDICATOR( port );
				break;
			}
			#endif

			default:
			{
				USBDEBUG( "__Set_Feature : Unknown Status (%lu)", LE_SWAP16( sd->Value ));
				break;
			}
		}
	}

	return( err );
}

// --
