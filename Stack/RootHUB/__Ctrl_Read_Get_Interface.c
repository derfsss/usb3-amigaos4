
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Get_Interface( 
		struct USB3_SetupData *sd, 
UNUSED	struct USB3_HCDNode *hn, 
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

	USBDEBUG( "Get Interface" );

	/**/ if	( sd->RequestType == ( REQTYPE_Read | REQTYPE_Standard | REQTYPE_Interface ))
	{
		if (( wValue == 0 ) && ( wIndex == HUB_Ifc_Nr ) && ( wLength == 1 ))
		{
			U8 *Altnr = buffer;
			Altnr[0]	= 0;
			*max		= 1;
			err			= USB3Err_NoError;
		}
	}

	return( err );
}

// --
