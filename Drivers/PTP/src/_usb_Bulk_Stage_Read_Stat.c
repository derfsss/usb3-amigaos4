
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

void _usb_Bulk_Stage_Read_Stat1( struct USB_Struct *us, struct USB_Command *cmd )
{
struct USB3_IORequest *ioreq;

	MYINFO( "PTP-USB : _usb_Bulk_Stage_Read_Stat1 : TransID %ld", us->us_TransactionID );

	cmd->ucmd_Next = _usb_Bulk_Stage_Read_Stat2;

	ioreq = us->us_Res_BulkIn->IORequests[0];
	ioreq->io_Data		= us->us_Res_BulkIn->Buffers[0];
	ioreq->io_Length	= us->us_Res_BulkIn->BufferSize;
	SendIO( (PTR) ioreq );
}

// --

void _usb_Bulk_Stage_Read_Stat2( struct USB_Struct *us, struct USB_Command *cmd )
{
struct USB3_IORequest *ioreq;
struct PTP_Container *con;

	MYINFO( "PTP-USB : _usb_Bulk_Stage_Read_Stat2 : TransID %ld", us->us_TransactionID );

	cmd->ucmd_Next = NULL;

	ioreq = us->us_Res_BulkIn->IORequests[0];

	con = (PTR) ioreq->io_Data;

	if ( ioreq->io_Length < 12 )
	{
		MYERROR( "PTP-USB : Invalid Reposnce 1 Length %ld", (S32) ioreq->io_Length );

		// hmm what do I do now?
	}
	else if ( LE_SWAP32( con->Length ) != 12 )
	{
		MYERROR( "PTP-USB : Invalid Reposnce 2 Length %ld", (S32) LE_SWAP32( con->Length ) );

		// hmm what do I do now?
	}
	else if ( LE_SWAP16( con->Type ) != CTYPE_Response )
	{
		MYERROR( "PTP-USB : Invalid Container Type %ld", (S32) LE_SWAP16( con->Type ) );

		// hmm what do I do now?
	}
	else if ( LE_SWAP32( con->TransID ) != us->us_TransactionID )
	{
		MYERROR( "PTP-USB : TransactionID mismatch $%08lx", (S32) LE_SWAP32( con->TransID ) );

		// hmm what do I do now?
	}
	else
	{
		/**/ if ( LE_SWAP16( con->Code ) == 0x201D )	// Busy
		{
			MYINFO( "PTP-USB : Busy delaying" );

			// Wait a 1/2 sec
			Delay( 25 );	

			// Try again
			_usb_Bulk_Stage_Send_Header1( us, cmd );
		}
		else
		{
			// Let Function validate Return code
			// and do its thing

			MYINFO( "PTP-USB : Okay" );

			if ( cmd->ucmd_Function )
			{
				cmd->ucmd_Function( us, cmd );
			}
		}
	}
}

// --
