
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Clr_Feature( 
		struct USB3_SetupData *sd, 
UNUSED	struct USB3_HCDNode *hn, 
UNUSED	struct USBBase *usbbase )
{
//U16 wLength;
U16 wValue;
U16 wIndex;
U16 port;
S32 err;

	err		= USB3Err_Host_Stall;
	wValue	= LE_SWAP16( sd->Value );
	wIndex	= LE_SWAP16( sd->Index );
//	wLength	= LE_SWAP16( sd->Length );
	port	= ( wIndex & 0x00ff );

	/**/ if	( sd->RequestType == ( REQTYPE_Write | REQTYPE_Class | REQTYPE_Device ))
	{
		// Clear HUB Status

		USBDEBUG( "__Clr_Feature : Clear HUB Staus??" );
	}
	else if ( sd->RequestType == ( REQTYPE_Write | REQTYPE_Class | REQTYPE_Other ))
	{
		// Clear Port Status

		USBDEBUG( "__Clr_Feature : Clear Port Feature on port #%lu", port );

		switch( wValue )
		{
			#if 0
			case PORT_ENABLE:
			{
				err = PORT_CLEAR_ENABLE( port );
				break;
			}

			case PORT_SUSPEND:
			{
				err = PORT_CLEAR_SUSPEND( port );
				break;
			}

			case PORT_POWER:
			{
				err = PORT_CLEAR_POWER( port );
				break;
			}

			case PORT_INDICATOR:
			{
				err = PORT_CLEAR_INDICATOR( port );
				break;
			}
			#endif

			case PORT_CLR_CONNECTION:
			{
//				err = PORT_CLEAR_CONNECTION_CHANGE( port );
				if ( hn->HCD_Functions.Port_Clr_Connection_Chg )
				{
					USBDEBUG( "__Clr_Feature : PORT_CLR_CONNECTION : Port #%ld", port );

					err = hn->HCD_Functions.Port_Clr_Connection_Chg( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Clr_Feature : PORT_CLR_CONNECTION : Not Implemented in HCD" );
					err = -1;
				}
				#endif
				break;
			}

			case PORT_CLR_ENABLE:
			{
//				err = PORT_CLEAR_ENABLE_CHANGE( port );
				if ( hn->HCD_Functions.Port_Clr_Enable_Chg )
				{
					USBDEBUG( "__Clr_Feature : PORT_CLR_ENABLE : Port #%ld", port );

					err = hn->HCD_Functions.Port_Clr_Enable_Chg( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Clr_Feature : PORT_CLR_ENABLE : Not Implemented in HCD" );
					err = -1;
				}
				#endif
				break;
			}

			case PORT_CLR_SUSPEND:
			{
//				err = PORT_CLEAR_SUSPEND_CHANGE( port );
				if ( hn->HCD_Functions.Port_Clr_Suspend_Chg )
				{
					USBDEBUG( "__Clr_Feature : PORT_CLR_SUSPEND : Port #%ld", port );

					err = hn->HCD_Functions.Port_Clr_Suspend_Chg( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Clr_Feature : PORT_CLR_SUSPEND : Not Implemented in HCD" );
					err = -1;
				}
				#endif
				break;
			}

			case PORT_CLR_OVER_CURRENT:
			{
//				err = PORT_CLEAR_OVERCURRENT_CHANGE( port );
				if ( hn->HCD_Functions.Port_Clr_OverCurrent_Chg )
				{
					USBDEBUG( "__Clr_Feature : PORT_CLR_OVER_CURRENT : Port #%ld", port );

					err = hn->HCD_Functions.Port_Clr_OverCurrent_Chg( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Clr_Feature : PORT_CLR_OVER_CURRENT : Not Implemented in HCD" );
					err = -1;
				}
				#endif
				break;
			}

			case PORT_CLR_RESET:
			{
//				err = PORT_CLEAR_RESET_CHANGE( port );
				if ( hn->HCD_Functions.Port_Clr_Reset_Chg )
				{
					USBDEBUG( "__Set_Feature : PORT_CLR_RESET : Port #%ld", port );

					err = hn->HCD_Functions.Port_Clr_Reset_Chg( hn, port );
				}
				#ifdef DO_DEBUG
				else
				{
					USBERROR( "__Set_Feature : PORT_CLR_RESET : Not Implemented in HCD" );
					err = -1;
				}
				#endif
				break;
			}

			default:
			{
				USBDEBUG( "__Clr_Feature : Unknown Status (%lu)", wValue );
				break;
			}
		}
	}

	return( err );
}

// --
