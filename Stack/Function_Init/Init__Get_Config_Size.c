
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Get_Config_Size( 
	struct RealFunctionNode *fn,
	struct USB3_IORequest *ioreq,
	struct USB3_SetupData *sd,
	struct USBBase *usbbase )
{
struct USB3_Config_Desc desc;
S32 retval;
U32 max;
U32 cnt;

	retval = FALSE;

	cnt = 0;
	max = 3;

	while( TRUE )
	{
		// Small delay (Retries and prev command)
//		HCD_DELAY_MS( 25 );

		// Check max retries
		if ( max < cnt++ )
		{
			USBERROR( "__Get_Config_Desc : Error Max trys : io_Error %ld", ioreq->io_Error );
			goto bailout;
		}

		sd->RequestType	= REQTYPE_Read | REQTYPE_Standard | REQTYPE_Device;
		sd->RequestCode	= REQCODE_Get_Descriptor;
		sd->Value		= LE_SWAP16( DSCTYPE_Configuration << 8 );
		sd->Index		= LE_SWAP16( 0 );
		sd->Length		= LE_SWAP16( sizeof( desc ));

		ioreq->io_Data	= & desc;
		ioreq->io_Length= sizeof( desc );

		IO_DO(ioreq);

		if ( ioreq->io_Error == USB3Err_NoError )
		{
			break;
		}
	}

	fn->fkt_Config_Desc_Size	= LE_SWAP16( desc.TotalLength );
	fn->fkt_Config_Desc_Buf		= MEM_ALLOCIOBUFFER( fn->fkt_Config_Desc_Size + sizeof( struct USB3_Descriptor ), TRUE );

	if ( ! fn->fkt_Config_Desc_Buf )
	{
		USBERROR( "Error allocating memory" );
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --
