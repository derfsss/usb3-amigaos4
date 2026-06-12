
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

U32 _usb_Init( struct USB_Struct *us )
{
U32 retval;

	MYINFO( "PTP-USB : _usb_Init" );

	retval = FALSE;
	
	// --

	NewList( & us->us_USBList );
	NewList( & us->us_CmdList );
	us->us_CmdListCnt = 0;

	us->us_CmdMsgPort = AllocSysObjectTags( ASOT_PORT, TAG_END );

	if ( ! us->us_CmdMsgPort )
	{
		MYERROR( "PTP-USB : Error creating MsgPort" );
		goto bailout;
	}

	// --

	us->us_Register = USB3_Reg_RegisterTags(
		USB3Tag_Reg_DriverMessage, us->us_StartMsg,
		TAG_END
	);

	if ( ! us->us_Register )
	{
		MYERROR( "PTP-USB : Error Registering Device" );
		goto bailout;
	}

	us->us_Res_Control = us->us_Register->Res_Control;

	// --

	us->us_Res_Interrupt = USB3_EPRes_ObtainTags( us->us_Register,
		USB3Tag_EPRes_EPDirection, EPADR_Dir_In,
		USB3Tag_EPRes_EPType, EPATT_Type_Interrupt,
		USB3Tag_EPRes_BufferSize, USB3Val_BufferSize_MaxPacketSize,
		TAG_END
	);

	if ( ! us->us_Res_Interrupt )
	{
		MYERROR( "PTP-USB : Error obtaining IntIn EndPoint Resource" );
		goto bailout;
	}

	// --

	us->us_Res_BulkIn = USB3_EPRes_ObtainTags( us->us_Register,
		USB3Tag_EPRes_BufferSize, USB3Val_BufferSize_MaxPacketSize,
		USB3Tag_EPRes_EPDirection, EPADR_Dir_In,
		USB3Tag_EPRes_EPType, EPATT_Type_Bulk,
		TAG_END
	);

	if ( ! us->us_Res_BulkIn )
	{
		MYERROR( "PTP-USB : Error obtaining BulkIn EndPoint Resource" );
		goto bailout;
	}

	// --

	us->us_Res_BulkOut = USB3_EPRes_ObtainTags( us->us_Register,
		USB3Tag_EPRes_BufferSize, USB3Val_BufferSize_MaxPacketSize,
		USB3Tag_EPRes_EPDirection, EPADR_Dir_Out,
		USB3Tag_EPRes_EPType, EPATT_Type_Bulk,
		USB3Tag_EPRes_AddZeroPacket, TRUE,
		TAG_END
	);

	if ( ! us->us_Res_BulkOut )
	{
		MYERROR( "PTP-USB : Error obtaining BulkOut EndPoint Resource" );
		goto bailout;
	}

	// --
	// Reset Start IDs
	us->us_SessionID = 1;
	us->us_TransactionID = 1;

	if ( ! _usb__Open_Session( us ))
	{
		MYERROR( "PTP-USB : Open Session failed" );
		goto bailout;
	}

	if ( ! _usb__Get_Device_Info( us ))
	{
		MYERROR( "PTP-USB : Get Device Info failed" );
		goto bailout;
	}

	MYERROR( "Yay We are setup" );

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --
