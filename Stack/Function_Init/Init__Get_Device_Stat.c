
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Get_Device_Stat( 
	struct RealFunctionNode *fn,
	struct USB2_IORequest *ioreq,
	struct USB2_SetupData *sd,
	struct USBBase *usbbase )
{
struct USB2_DeviceStatus stat;
S32 retval;
U32 max;
U32 cnt;

	retval = FALSE;

	// Now we know the Real EndPoint Size
	// so get Full Device Descriptor

	cnt = 0;
	max = 3;

	while( TRUE )
	{
		// Small delay (Retries and prev command)
//		HCD_DELAY_MS( 25 );

		// Check max retries
		if ( max < cnt++ )
		{
			USBERROR( "__Get_Device_Desc : Error Max trys : io_Error %ld", ioreq->io_Error );
			goto bailout;
		}

		sd->RequestType	= REQTYPE_Read | REQTYPE_Standard | REQTYPE_Device;
		sd->RequestCode	= REQCODE_Get_Status;
		sd->Value		= LE_SWAP16( 0 );
		sd->Index		= LE_SWAP16( 0 );
		sd->Length		= LE_SWAP16( sizeof( stat ));

		ioreq->io_Data	= & stat ;
		ioreq->io_Length= sizeof( stat );

		IO_DO(ioreq);

		if ( ioreq->io_Error == USB2Err_NoError )
		{
			break;
		}
	}

	if ( stat.Stat & USBSTATUS_Device_SelfPowered )
	{
		// The SelfPowered bit may not change so we can store the result
		fn->fkt_SelfPowered = TRUE;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --
