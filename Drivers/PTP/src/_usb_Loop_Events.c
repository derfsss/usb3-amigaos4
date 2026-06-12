
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

void _usb_Loop_Event( struct USB_Struct *us )
{
struct USB3_IORequest *ioreq;
struct PTP_Container *condata;

	while( TRUE )
	{
		ioreq = (PTR) GetMsg( us->us_Res_Interrupt->MsgPort );

		if ( ! ioreq )
		{
			break;
		}

		us->us_Errors_Int++;

		/**/ if ( ioreq->io_Error == USB3Err_Device_Detached )
		{
			us->us_Detached = TRUE;
			break;
		}
		else if ( ioreq->io_Error == USB3Err_Host_Stall )
		{
			USB3_EPRes_Destall( us->us_Res_Interrupt );
		}
		else if ( ioreq->io_Error == USB3Err_NoError )
		{
			condata = ioreq->io_Data;

			if (( BE_SWAP32( ioreq->io_Length ) >= 12 ) 
			&&	( LE_SWAP32( condata->Length ) >= 12 )
			&&	( LE_SWAP16( condata->Type ) == CTYPE_Event ))
			{
				us->us_Errors_Int = 0;

				switch( LE_SWAP16( condata->Code ))
				{
					default:
					{
						MYERROR( "Unsupported event code : $%04lx", LE_SWAP16( condata->Code ));
						break;
					}
				}
			}
		}

		if (( ! us->us_Detached ) && ( us->us_Errors_Int < 20 ))
		{
			// Ask for next event
			SendIO( (PTR) ioreq );
		}
	}
}

// --
