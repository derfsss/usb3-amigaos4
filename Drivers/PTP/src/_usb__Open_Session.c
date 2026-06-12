
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

static void ucmd_Handle_Open_Session( struct USB_Struct *us, struct USB_Command *cmd );

// --

U32 _usb__Open_Session( struct USB_Struct *us )
{
struct PTP_Container *con;
struct USB_Command *cmd;
U32 retval;

	MYINFO( "PTP-USB : _usb__Open_Session" );

	retval = FALSE;

	cmd = _usb_Bulk_Cmd_Get( us );

	if ( ! cmd )
	{
		MYERROR( "PTP-USB : _usb_Bulk_Cmd_Get() failed" );
		goto bailout;
	}

	cmd->ucmd_Function = ucmd_Handle_Open_Session;
	cmd->ucmd_Type = CMDTYPE_Cmd_None_Stat;

	// Open Session
	con = & cmd->ucmd_Container;
	con->Length		= LE_SWAP32( 12 + 1*4 );
	con->Type		= LE_SWAP16( CTYPE_Command );
	con->Code		= LE_SWAP16( PTP_Open_Session );
//	con->TransID	= LE_SWAP32( us->us_TransactionID );
	con->Parms[0]	= LE_SWAP32( us->us_SessionID );

	AddTail( & us->us_USBList, & cmd->ucmd_Node );

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void ucmd_Handle_Open_Session( struct USB_Struct *us, struct USB_Command *cmd )
{
struct USB3_IORequest *ioreq;
struct PTP_Container *con;

	MYINFO( "PTP-USB : ucmd_Handle_Open_Session" );

	ioreq	= us->us_Res_BulkIn->IORequests[0];
	con		= ioreq->io_Data;

	if ( LE_SWAP16( con->Code ) != 0x2001 )
	{
		MYERROR( "PTP-USB : Returned error $%04lx", (S32) LE_SWAP16( con->Code ) );

		us->us_Running = FALSE;
	}
}

// --
