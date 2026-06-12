
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __Get_EndPoint_Size( 
	struct USB2_EPResource *epr,
	struct RealFunctionNode *fn,
	struct USB2_IORequest *ioreq,
	struct USB2_SetupData *sd,
	struct USBBase *usbbase )
{
S32 retval;
U32 max;
U32 cnt;

	retval = FALSE;

	// Only get 8 bytes of the Device Descriptor
	// so we can get the EndPoint Size

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

		sd->RequestType			= REQTYPE_Read | REQTYPE_Standard | REQTYPE_Device;
		sd->RequestCode			= REQCODE_Get_Descriptor;
		sd->Value				= LE_SWAP16( DSCTYPE_Device << 8 );
		sd->Index				= LE_SWAP16( 0 );
		sd->Length				= LE_SWAP16( 8 );

		ioreq->io_Data			= fn->fkt_DeviceDescriptor;
		ioreq->io_Length		= 8;
		ioreq->io_Command		= CMD_READ;
		ioreq->io_SetupData		= sd;
		ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

		IO_DO(ioreq);

		if ( ioreq->io_Error == USB2Err_NoError )
		{
			break;
		}
	}

	// Update Control EndPoint PacketSize
	((struct USB2_EndPointNode *)epr->EndPoint)->ep_MaxPacketSize = fn->fkt_DeviceDescriptor->MaxPacketSize0;

	retval = TRUE;

bailout:

	return( retval );
}

// --
