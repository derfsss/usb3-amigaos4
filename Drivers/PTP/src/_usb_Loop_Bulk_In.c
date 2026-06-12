
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

void _usb_Loop_Bulk_In( struct USB_Struct *us )
{
struct USB3_IORequest *ioreq;
struct USB_Command *cmd;

	MYINFO( "PTP-USB : _usb_Loop_Bulk_In" );

	while( TRUE )
	{
		ioreq = (PTR) GetMsg( us->us_Res_BulkIn->MsgPort );

		if ( ! ioreq )
		{
			break;
		}

		us->us_Errors_Int++;

		/**/ if ( ioreq->io_Error == USB3Err_Device_Detached )
		{
			MYINFO( "PTP-USB : USB3Err_Device_Detached" );
			us->us_Detached = TRUE;
			break;
		}
		else if ( ioreq->io_Error == USB3Err_Host_Stall )
		{
			MYINFO( "PTP-USB : USB3Err_Host_Stall" );
			USB3_EPRes_Destall( us->us_Res_BulkIn );
			USB3_EPRes_Destall( us->us_Res_BulkOut );
		}
		else if ( ioreq->io_Error == USB3Err_NoError )
		{
			us->us_Errors_Bulk = 0;

			cmd = us->us_USBActive;

			if ( cmd->ucmd_Next )
			{
				MYINFO( "PTP-USB : ucmd_Next %p", cmd->ucmd_Next );
				cmd->ucmd_Next( us, cmd );
			}

			if ( ! cmd->ucmd_Next )
			{
				MYINFO( "PTP-USB : ucmd_Next %p", cmd->ucmd_Next );
				us->us_TransactionID++;
				us->us_USBActive = NULL;
				_usb_Bulk_Cmd_Free( us, cmd );
			}
		}
		else
		{
			MYINFO( "PTP-USB : Unknown error" );
		}
	}
}

// --
