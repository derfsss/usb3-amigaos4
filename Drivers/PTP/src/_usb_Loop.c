
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

void _usb_Loop( struct USB_Struct *us )
{
struct USB3_IORequest *ioreq;
struct CommandMessage *cm;
U32 wait;
U32 mask;

	MYINFO( "PTP-USB : _usb_Loop" );

	// --

	cm = AllocVecTags( sizeof( struct CommandMessage ),
		AVT_Clear, 0,
		TAG_END
	);

	if ( cm )
	{
		us->us_CameraNode.cin_Name = "Hello World";
		us->us_CameraNode.cin_MsgPort = us->us_CmdMsgPort;
		// --
		cm->cm_Command = CMD_Camera_Added;
		cm->cm_InfoNode = & us->us_CameraNode;
		PutMsg( FS_CmdMsgPort, (PTR) cm );
	}

	// --
	// Start Reading from Events

	ioreq = us->us_Res_Interrupt->IORequests[0];
	ioreq->io_AllowShortPackets = TRUE;
	SendIO( (PTR) ioreq );

	// --
	// Start Bulk transfers

	_usb_Bulk_Schedule( us );

	// --

	wait = SIGBREAKF_CTRL_C;
	wait |=	us->us_Res_BulkIn->MsgPortBit;
	wait |=	us->us_Res_BulkOut->MsgPortBit;
	wait |=	us->us_Res_Interrupt->MsgPortBit;
	wait |=	us->us_Register->Stack_MsgPortBit;

	MYINFO( "PTP-USB : BulkIn ..... : $%08lx", us->us_Res_BulkIn->MsgPortBit );
	MYINFO( "PTP-USB : BulkOut .... : $%08lx", us->us_Res_BulkOut->MsgPortBit );
	MYINFO( "PTP-USB : Interrupt .. : $%08lx", us->us_Res_Interrupt->MsgPortBit );
	MYINFO( "PTP-USB : Stack ...... : $%08lx", us->us_Register->Stack_MsgPortBit );
	MYINFO( "PTP-USB : Stack MP ... : %p", us->us_Register->Stack_MsgPort );

	us->us_Running = TRUE;

	while(( us->us_Running ) && ( ! us->us_Detached ))
	{
		MYINFO( "PTP-USB : Waiting for $%08lx", wait );

		mask = Wait( wait );

		MYINFO( "PTP-USB : Got $%08lx", mask );

		if ( mask &	us->us_Res_BulkIn->MsgPortBit )
		{
			MYINFO( "PTP-USB : Got BulkIn Bit" );
			_usb_Loop_Bulk_In( us );
			_usb_Bulk_Schedule( us );
		}

		if ( mask &	us->us_Res_BulkOut->MsgPortBit )
		{
			MYINFO( "PTP-USB : Got BulkOut Bit" );
			_usb_Loop_Bulk_Out( us );
			_usb_Bulk_Schedule( us );
		}

		if ( mask &	us->us_Res_Interrupt->MsgPortBit )
		{
			MYINFO( "PTP-USB : Got Interrupt Bit" );
			_usb_Loop_Event( us );
		}

		if ( mask &	us->us_Register->Stack_MsgPortBit )
		{
			MYINFO( "PTP-USB : Got Stack Bit" );
			_usb_Loop_Stack( us );
		}

		if ( mask & SIGBREAKF_CTRL_C )
		{
			MYINFO( "PTP-USB : Got CTRL+C" );
			us->us_Running = FALSE;
		}
	}

	// --

	cm = AllocVecTags( sizeof( struct CommandMessage ),
		AVT_Clear, 0,
		TAG_END
	);

	if ( cm )
	{
		cm->cm_Command = CMD_Camera_Removed;
		cm->cm_InfoNode = & us->us_CameraNode;
		PutMsg( FS_CmdMsgPort, (PTR) cm );
	}

	// --

}

// --
