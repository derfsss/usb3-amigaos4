
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"
#include "HID.h"

// --

SEC_CODE S32 HID_Init( struct USBBase *usbbase, struct intern *in )
{
struct USB2_Interface_Desc *ifcdsc;
struct USB2_IORequest *ioreq;
//struct USB2_SetupData *sd;
S32 retval;
S32 ifcnr;
U32 cnt;

	retval = FALSE;

	TASK_NAME_ENTER( "HID_Init" );

	usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: 1 MsgPort init\n" );

	if ( ! MSGPORT_INIT( & in->Input_MsgPort ))
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: MsgPort init FAILED\n" );
		goto bailout;
	}

	MEM_COPY( usbbase->usb_InputIORequest, & in->Input_IOReq, sizeof( struct IOStdReq ));
	in->Input_IOReq.io_Message.mn_ReplyPort = & in->Input_MsgPort.ump_MsgPort;

	usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: 2 Register\n" );

	in->Register = REGISTER_REGISTERTAGS(
		USB2Tag_Reg_DriverMessage, in->StartMsg,
//		USB2Tag_Reg_TimeOut, 1000000*1,		// 1 sec
		USB2Tag_Reg_Title, "HID",
		TAG_END
	);

	if ( ! in->Register )
	{
		USBDEBUG( "HID_Init : Error Registering Device" );
		goto bailout;
	}

	usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: 3 Register OK=%p\n", in->Register );

	in->Res_Control = in->Register->reg_Public.Res_Control;
	ifcdsc	= in->StartMsg->InterfaceDescriptor;
	ioreq	= in->Res_Control->IORequests[0];
	ifcnr	= ifcdsc->InterfaceNumber;

	usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: 4 Obtain interrupt EP (ifcnr=%ld)\n", ifcnr );

	in->Res_Interrupt = ENDPOINTRES_OBTAINTAGS( in->Register,
		USB2Tag_EPRes_EPType, EPATT_Type_Interrupt,
		USB2Tag_EPRes_EPDirection, EPADR_Dir_In,
		USB2Tag_EPRes_NrOfIORequest, HID_IOReqCount,
		USB2Tag_EPRes_InterfaceNr, ifcnr,
		USB2Tag_EPRes_BufferSize, USB2Val_BufferSize_MaxPacketSize,
		TAG_END
	);

	usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: 5 Res_Interrupt=%p\n", in->Res_Interrupt );

	if ( ! in->Res_Interrupt )
	{
		usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: Interrupt EP FAILED\n" );
		goto bailout;
	}

	// --

	// --
	//  Switch to Report Protocol

	// Is it running in BOOT Protocol mode
	if ( ifcdsc->InterfaceSubClass != USBHID_SUBCLASS_REPORT )
	{
		USBDEBUG( "HID_Init : Trying to switch to Report Mode" );

		#if 0
		// ah it is lets try and change it to Report.
		sd->RequestType = REQTYPE_Write | REQTYPE_Class | REQTYPE_Interface;
		sd->RequestCode = REQCODE_Set_Protocol;
		sd->Value		= LE_SWAP16( 1 );		// Report Protocol
		sd->Index		= LE_SWAP16( ifcnr );
		sd->Length		= LE_SWAP16( 0 );

		ioreq->io_Command		= CMD_WRITE;
		ioreq->io_Data			= NULL;
		ioreq->io_Length		= 0;
		ioreq->io_SetupData		= sd;
		ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

		IO_DO( ioreq );

		if ( ioreq->io_Error == USB2Err_NoError )
		{
			in->Driver_Mode = HID_DMode_Report;
		}
		else
		#endif
		{
			in->Driver_Mode = HID_DMode_Boot;
		}
	}
	else
	{
		in->Driver_Mode = HID_DMode_Report;
	}

	// -- Check Supported modes

	usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: 6 mode=%ld\n", in->Driver_Mode );

	/**/ if ( in->Driver_Mode == HID_DMode_Boot )
	{
		/**/ if ( ifcdsc->InterfaceProtocol == USBHID_PROTOCOL_KEYBOARD )
		{
			usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: Boot Keyboard!\n" );
			in->Driver_Type = HID_DType_Keyboard;
		}
		else if ( ifcdsc->InterfaceProtocol == USBHID_PROTOCOL_MOUSE )
		{
			usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: Boot Mouse!\n" );
			in->Driver_Type = HID_DType_Mouse;
		}
		else
		{
			usbbase->usb_IExec->DebugPrintF( "USB3: HID_Init: Unsupported boot protocol %ld\n", (U32) ifcdsc->InterfaceProtocol );
			goto bailout;
		}
	}
	else if ( in->Driver_Mode == HID_DMode_Report )
	{
		USBDEBUG( "HID_Init : HID Report mode not supported (yet)" );
		goto bailout;
	}
	else
	{
		USBDEBUG( "HID_Init : Error unknown HID Device type" );
		goto bailout;
	}

	// -- Misc Alloc pr Mode

	if (( in->Driver_Mode == HID_DMode_Boot ) && ( in->Driver_Type == HID_DType_Keyboard ))
	{
		if ( ! MSGPORT_INIT( & in->Type.Boot_Keyboard.Timer_MsgPort ))
		{
			USBDEBUG( "HID_Init : Boot : Keyboard : Timer MsgPort init failed" );
			goto bailout;
		}

		MEM_COPY( usbbase->usb_TimeRequest, & in->Type.Boot_Keyboard.Timer_IOReq, sizeof( struct TimeRequest ));
		in->Type.Boot_Keyboard.Timer_IOReq.Request.io_Message.mn_ReplyPort = & in->Type.Boot_Keyboard.Timer_MsgPort.ump_MsgPort;
	}

	// --


	// --
	// Just start to Read from Interrupts
	// Hmm should I move this

	for( cnt=0 ; cnt<HID_IOReqCount ; cnt++ )
	{
		ioreq = in->Res_Interrupt->IORequests[cnt];
		ioreq->io_Data		= in->Res_Interrupt->Buffers[cnt];
		ioreq->io_Length	= in->Res_Interrupt->BufferSize;
		ioreq->io_Command	= CMD_READ;
		IO_SEND( ioreq );
	}

	// --

	retval = TRUE;

	// --

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
