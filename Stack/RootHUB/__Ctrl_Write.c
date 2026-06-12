
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "__Ctrl_Write_Clr_Feature.c"
#include "__Ctrl_Write_Set_Config.c"
#include "__Ctrl_Write_Set_Feature.c"

// --

SEC_CODE static void __Control_Write( struct USBBase *usbbase, struct USB3_HCDNode *hn, struct RealRequest *ioreq )
{
struct USB3_SetupData *sd;
S32 err;

	sd = ioreq->req_Public.io_SetupData;

	if ( ! sd )
	{
		USBDEBUG( "RootHUB : Write : No SetupData buffer given" );

		err = USB3Err_Host_Stall;
		return;
	}
	else
	{
		switch( sd->RequestCode )
		{
			case REQCODE_Set_Feature:
			{
				USBDEBUG( "RootHUB : Write : Set Feature" );

				err = __Set_Feature( sd, hn, usbbase );
				break;
			}

			case REQCODE_Clear_Feature:
			{
				USBDEBUG( "RootHUB : Write : Clear Feature" );

				err = __Clr_Feature( sd, hn, usbbase );
				break;
			}

			case REQCODE_Set_Configuration:
			{
				USBDEBUG( "RootHUB : Write : Set Configuration Feature" );

				err = __Set_Config( sd, hn, usbbase );
				break;
			}

			default:
			{
				USBDEBUG( "RootHUB : Write : Unknown Request (%lu)", (U32) sd->RequestCode );

				err = USB3Err_Host_Stall;
				break;
			}
		}
	}

	ioreq->req_Public.io_Error = err;
}

// --
