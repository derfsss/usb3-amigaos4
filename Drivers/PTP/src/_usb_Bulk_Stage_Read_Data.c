
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

void _usb_Bulk_Stage_Read_Data2( struct USB_Struct *us, struct USB_Command *cmd );
void _usb_Bulk_Stage_Read_Data3( struct USB_Struct *us, struct USB_Command *cmd );

// --

void _usb_Bulk_Stage_Read_Data1( struct USB_Struct *us, struct USB_Command *cmd )
{
struct USB3_IORequest *ioreq;

	MYINFO( "PTP-USB : _usb_Bulk_Stage_Read_Data1 : TransID %ld", us->us_TransactionID );
	MYINFO( "PTP-USB : _usb_Bulk_Stage_Read_Data1 : MaxP %ld", us->us_Res_BulkIn->EndPointMax );

	cmd->ucmd_Next = _usb_Bulk_Stage_Read_Data2;
	cmd->ucmd_Data_Current = 0;

	ioreq = us->us_Res_BulkIn->IORequests[0];
	ioreq->io_Data		= us->us_Res_BulkIn->Buffers[0];
	ioreq->io_Length	= us->us_Res_BulkIn->EndPointMax;
	SendIO( (PTR) ioreq );
}

// --

void _usb_Bulk_Stage_Read_Data2( struct USB_Struct *us, struct USB_Command *cmd )
{
struct USB3_IORequest *ioreq;
struct PTP_Container *con;
U32 left;
U32 len;
U8 *data;

	MYINFO( "PTP-USB : _usb_Bulk_Stage_Read_Data2 : TransID %ld", us->us_TransactionID );

//	cmd->ucmd_Next = _usb_Bulk_Stage_Read_Stat2;

	ioreq = us->us_Res_BulkIn->IORequests[0];

	con = (PTR) ioreq->io_Data;

	if ( ioreq->io_Length < 12 )
	{
		MYERROR( "PTP-USB : Invalid Reposnce 1 Length %ld", (S32) ioreq->io_Length );

		// hmm what do I do now?
	}
	else if ( LE_SWAP32( con->Length ) < 12 )
	{
		MYERROR( "PTP-USB : Invalid Reposnce 2 Length %ld", (S32) LE_SWAP32( con->Length ) );

		// hmm what do I do now?
	}
	else if ( LE_SWAP32( con->TransID ) != us->us_TransactionID )
	{
		MYERROR( "PTP-USB : TransactionID mismatch $%08lx", (S32) LE_SWAP32( con->TransID ) );

		// hmm what do I do now?
	}
	else if ( LE_SWAP16( con->Type ) == CTYPE_Response )
	{
		if ( LE_SWAP16( con->Code ) == 0x201D )	// Busy
		{
			MYINFO( "PTP-USB : Busy delaying" );

			// Wait a 1/2 sec
			Delay( 25 );	

			// Try again
			_usb_Bulk_Stage_Send_Header1( us, cmd );
		}
		else
		{
			MYERROR( "PTP-USB : Error Code $%04lx", (S32) LE_SWAP16( con->Code ) );

			// hmm what do I do now?
		}
	}
	else if ( LE_SWAP16( con->Type ) != CTYPE_Data )
	{
		MYERROR( "PTP-USB : Invalid Container Type %ld", (S32) LE_SWAP16( con->Type ) );

		// hmm what do I do now?
	}
	else
	{
		cmd->ucmd_Data_Length = LE_SWAP32( con->Length ) - 12;
		cmd->ucmd_Data_Buffer = AllocVecTags( cmd->ucmd_Data_Length,
			TAG_END
		);

		if ( ! cmd->ucmd_Data_Buffer )
		{
			MYERROR( "PTP-USB : Out of mem %ld", cmd->ucmd_Data_Length );

			// hmm what do I do now?
		}
		else
		{
			cmd->ucmd_Next = _usb_Bulk_Stage_Read_Data3;

			ioreq = us->us_Res_BulkIn->IORequests[0];
			data = ioreq->io_Data;

			// How many bytes left in the buffer
			left = cmd->ucmd_Data_Length - cmd->ucmd_Data_Current;

			// We only read one MaxPacket at a time
			len	= MIN( us->us_Res_BulkIn->EndPointMax, left );

			// Make sure we skip Header
			len	= MIN( ioreq->io_Length-12, len );

			// Copy Data minus header
			memcpy( & cmd->ucmd_Data_Buffer[ cmd->ucmd_Data_Current ], & data[12], len );

			cmd->ucmd_Data_Current += len;

			if ( cmd->ucmd_Data_Current < cmd->ucmd_Data_Length )
			{
				// Read one more Max Packet buffer
				SendIO( (PTR) ioreq );
			}
			else
			{
				_usb_Bulk_Stage_Read_Stat1( us, cmd );
			}
		}
	}
}

// --

void _usb_Bulk_Stage_Read_Data3( struct USB_Struct *us, struct USB_Command *cmd )
{
struct USB3_IORequest *ioreq;
U32 left;
U32 len;
U8 *data;

	MYINFO( "PTP-USB : _usb_Bulk_Stage_Read_Data3" );

	ioreq = us->us_Res_BulkIn->IORequests[0];
	data = ioreq->io_Data;

	// How many bytes left in the buffer
	left = cmd->ucmd_Data_Length - cmd->ucmd_Data_Current;

	// We only read one MaxPacket at a time
	len	= MIN( us->us_Res_BulkIn->EndPointMax, left );

	// it may be last chunky that is less than MaxPacket
	len	= MIN( ioreq->io_Length, len );

	memcpy( & cmd->ucmd_Data_Buffer[ cmd->ucmd_Data_Current ], data, len );

	cmd->ucmd_Data_Current += len;

	if ( cmd->ucmd_Data_Current < cmd->ucmd_Data_Length )
	{
		// Read one more Max Packet buffer
		SendIO( (PTR) ioreq );
	}
	else
	{
		_usb_Bulk_Stage_Read_Stat1( us, cmd );
	}
}

// --

