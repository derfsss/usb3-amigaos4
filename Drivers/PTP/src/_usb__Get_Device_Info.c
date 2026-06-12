
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --


/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

static void ucmd_Handle_Device_Info( struct USB_Struct *us, struct USB_Command *cmd );

// --

U32 _usb__Get_Device_Info( struct USB_Struct *us )
{
struct PTP_Container *con;
struct USB_Command *cmd;
U32 retval;

	MYINFO( "PTP-USB : _usb__Get_Device_Info" );

	retval = FALSE;

	cmd = _usb_Bulk_Cmd_Get( us );

	if ( ! cmd )
	{
		MYERROR( "PTP-USB : _usb_Bulk_Cmd_Get() failed" );
		goto bailout;
	}

	cmd->ucmd_Function = ucmd_Handle_Device_Info;
	cmd->ucmd_Type = CMDTYPE_Cmd_ReadMem_Stat;

	con = & cmd->ucmd_Container;
	con->Length		= LE_SWAP32( 12 + 0*4 );
	con->Type		= LE_SWAP16( CTYPE_Command );
	con->Code		= LE_SWAP16( PTP_Get_Device_Info );
//	con->TransID	= LE_SWAP32( us->us_TransactionID );

	AddTail( & us->us_USBList, & cmd->ucmd_Node );

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void ucmd_Handle_Device_Info( struct USB_Struct *us, struct USB_Command *cmd )
{
struct USB3_IORequest *ioreq;
struct PTP_Container *con;

	MYINFO( "PTP-USB : ucmd_Handle_Device_Info" );

	ioreq	= us->us_Res_BulkIn->IORequests[0];
	con		= ioreq->io_Data;

	if ( LE_SWAP16( con->Code ) != 0x2001 )
	{
		MYERROR( "PTP-USB : Returned error $%04lx", (S32) LE_SWAP16( con->Code ) );

		us->us_Running = FALSE;
	}
}

// --

#if 0

U32 _usb__Get_Device_Info( struct USB_Struct *us )
{
struct USB3_IORequest *ioreq;
struct PTP_Container *condata;
U32 retval;
U8 data[512];

	MYINFO( "PTP-USB : _usb__Get_Device_Info" );

	retval = FALSE;

	// Only add to TransID at start cmd, like here
	us->us_TransactionID++;

	// Get Device Info
	condata = (PTR) data;
	condata->Length		= LE_SWAP32( 12 + 0*4 );
	condata->Type		= LE_SWAP16( CTYPE_Command );
	condata->Code		= LE_SWAP16( PTP_Get_Device_Info );
	condata->TransID	= LE_SWAP32( us->us_TransactionID );

	// Send
	ioreq = us->us_Res_BulkOut->IORequests[0];
	ioreq->io_Data = condata;
	ioreq->io_Length = 12 + 0*4;
	DoIO( (PTR) ioreq );

	if ( ioreq->io_Error )
	{
		MYERROR( "PTP-USB : Error sending %ld", (S32) ioreq->io_Error );
		goto bailout;
	}


	
	// Read data
	ioreq = us->us_Res_BulkIn->IORequests[0];
	ioreq->io_Data = data;
	ioreq->io_Length = 512;		// fix me: hard coded maxpacket size (its ehci)

	S32 full;
	S32 once = TRUE;
	S32 llen = 0;
	PTR buf = NULL;
	PTR cur;

	do
	{
		DoIO( (PTR) ioreq );

		if ( ioreq->io_Error )
		{
			MYERROR( "PTP-USB : Error reading %ld", (S32) ioreq->io_Error );
			goto bailout;
		}

		if ( once )
		{
			once = FALSE;

			// Need header
			if ( ioreq->io_Length < 12 )
			{
				MYERROR( "PTP-USB : Invalid Reposnce Length %ld", (S32) ioreq->io_Length );
				goto bailout;
			}

			// Removed header, so we have real data size, recived
			ioreq->io_Length -= 12;

			condata = (PTR) ioreq->io_Data;

			if ( LE_SWAP16( condata->Type ) != CTYPE_Data )
			{
				MYERROR( "PTP-USB : Invalid Container Type %ld", (S32) LE_SWAP16( condata->Type ) );
				goto bailout;
			}

			if ( LE_SWAP32( condata->TransID ) != us->us_TransactionID )
			{
				MYERROR( "PTP-USB : TransactionID mismatch $%08lx", (S32) LE_SWAP32( condata->TransID ) );
				goto bailout;
			}

			// get full data buffer size
			llen = LE_SWAP32( condata->Length ) - 12;
			full = llen;

			cur = buf = AllocVecTags( llen,
				TAG_END
			);

			if ( ! buf )
			{
				MYERROR( "PTP-USB : Out of mem %ld", (S32) llen );
				goto bailout;
			}

			memcpy( buf, & data[12], MIN( llen, (S32) ioreq->io_Length ));
		}
		else
		{
			memcpy( cur, data, MIN( llen, (S32) ioreq->io_Length ));
		}

		llen	-= ioreq->io_Length;
		cur		+= ioreq->io_Length;
		MYERROR( "PTP-USB : Data %ld bytes now %ld/%ld", (S32) ioreq->io_Error, llen, full );
	}
	while( llen > 0 );

	// --
	// buf = should hold data now

	// Read responce
	ioreq = us->us_Res_BulkIn->IORequests[0];
	DoIO( (PTR) ioreq );

	if ( ioreq->io_Error )
	{
		MYERROR( "PTP-USB : Error reading %ld", (S32) ioreq->io_Error );
		goto bailout;
	}

	if ( ioreq->io_Length < 12 )
	{
		MYERROR( "PTP-USB : Invalid Reposnce Length %ld", (S32) ioreq->io_Length );
		goto bailout;
	}

	condata = (PTR) ioreq->io_Data;

	if ( LE_SWAP32( condata->Length ) != 12 )
	{
		MYERROR( "PTP-USB : Invalid Reposnce 2 Length %ld", (S32) LE_SWAP32( condata->Length ) );
		goto bailout;
	}

	if ( LE_SWAP16( condata->Type ) != CTYPE_Response )
	{
		MYERROR( "PTP-USB : Invalid Container Type %ld", (S32) LE_SWAP16( condata->Type ) );
		goto bailout;
	}

	if ( LE_SWAP32( condata->TransID ) != us->us_TransactionID )
	{
		MYERROR( "PTP-USB : TransactionID mismatch $%08lx", (S32) LE_SWAP32( condata->TransID ) );
		goto bailout;
	}

	if ( LE_SWAP16( condata->Code ) != 0x2001 )
	{
		MYERROR( "PTP-USB : Returned error $%04lx", (S32) LE_SWAP16( condata->Code ) );
		goto bailout;
	}

	// --

	MYERROR( "Yay Device Info done" );

	// --

	retval = TRUE;

bailout:

	return( retval );
}

#endif
// --
