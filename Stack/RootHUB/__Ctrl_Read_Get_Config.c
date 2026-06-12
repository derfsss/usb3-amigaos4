
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Get_Config( 
		struct USB3_SetupData *sd, 
		struct USB3_HCDNode *hn,
UNUSED	struct USBBase *usbbase,
		PTR buffer, 
		U32 *max )
{
U16 wLength;
U16 wValue;
U16 wIndex;
S32 err;

	err		= USB3Err_Host_Stall;
	wValue	= LE_SWAP16( sd->Value );
	wIndex	= LE_SWAP16( sd->Index );
	wLength	= LE_SWAP16( sd->Length );

	/**/ if	( sd->RequestType == ( REQTYPE_Read | REQTYPE_Standard | REQTYPE_Device ))
	{
		if (( wValue == 0 ) && ( wIndex == 0 ) && ( wLength == 1 ))
		{
			U8 *Cfgnr = buffer;

			Cfgnr[0]	= hn->hn_HUB_ConfigNr;
			USBDEBUG( "__Get_Config: Get Config Nr #%lu", (U32) Cfgnr[0] );
			*max		= 1;
			err			= USB3Err_NoError;
		}
		else
		{
			USBDEBUG( "__Get_Config : Unknonw 2" );
		}
	}
	else
	{
		USBDEBUG( "__Get_Config : Unknonw 1" );
	}

	return( err );
}

// --
