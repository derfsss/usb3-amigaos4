
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Set_Config( 
		struct USB3_SetupData *sd, 
		struct USB3_HCDNode *hn, 
UNUSED	struct USBBase *usbbase )
{
U16 wLength;
U16 wValue;
U16 wIndex;
S32 err;

	err		= USB3Err_Host_Stall;
	wValue	= LE_SWAP16( sd->Value );
	wIndex	= LE_SWAP16( sd->Index );
	wLength	= LE_SWAP16( sd->Length );

	if (( wValue == HUB_Cfg_Nr ) && ( wIndex == 0 ) && ( wLength == 0 ))
	{
		USBDEBUG( "__Set_Config : Setting Config Nr #%ld", (S32) wValue );
		hn->hn_HUB_ConfigNr = wValue;
		err = USB3Err_NoError;
	}
	else
	{
		USBDEBUG( "__Set_Config : hmm unknown 11" );
		err = USB3Err_Host_Stall;
	}

	return( err );
}

// --
