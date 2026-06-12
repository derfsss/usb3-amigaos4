
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

// 1 : Get CTRL EP Size
#include "Init__Get_EndPoint_Size.c"

// 2 : Get Device Descriptor
#include "Init__Get_Device_Desc.c"

// 3 : Get Device Stat
#include "Init__Get_Device_Stat.c"

// 4 : Get Config Size
#include "Init__Get_Config_Size.c"

// 5 : Get Config Descriptor
#include "Init__Get_Config_Desc.c"

// 6 : Get Lang/Strings
#include "Init__Get_Strings.c"

// --

SEC_CODE static void __myMain( struct USBBase *usbbase, PTR userdata, PTR in UNUSED )
{
struct USB3_EPResource *epr;
struct RealFunctionNode *fn;
struct USB3_IORequest *ioreq;
struct USB3_SetupData *sd;
struct RealRegister *reg;

	fn = userdata;

	#ifdef DO_INFO
{
	struct ExecIFace *IExec = usbbase->usb_IExec;
	IExec->Disable();
	IExec->DebugPrintF( "\nStartup new Init\n" );
	IExec->DebugPrintF( "HCD Index   : %ld\n", fn->fkt_HCD->hn_HCDIndex );
	IExec->DebugPrintF( "Tier        : %ld\n", fn->fkt_Tier );
	IExec->DebugPrintF( "Addr Nr     : %ld\n", fn->fkt_Address );
	IExec->DebugPrintF( "Port Nr     : %ld\n\n", fn->fkt_PortNr );
	IExec->Enable();
}
	#endif

	USBDEBUG( "__myMain (Init)          : Enter : FN    %p", fn );

	reg = NULL;

	// --

	if ( FUNCTION_VALID(fn) != VSTAT_Okay )
	{
		USBERROR( "__myMain (Init)          : Invalid FN (%p)", fn );
		goto bailout;
	}

	// --

	reg = REGISTER_REGISTERTAGS(
		USB3Tag_Reg_Function, fn,
		USB3Tag_Reg_TimeOut, 2 * 1000000,
		USB3Tag_Reg_Title, "Init",
		TAG_END
	);

	if ( REGISTER_VALID( reg ) != VSTAT_Okay )
	{
		USBERROR( "__myMain (Init)          : Error Registering Device" );
		goto bailout;
	}

	// --

	USBDEBUG( "__myMain (Init) Running" );
//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY 1 - Register\n##\n\n" );

	epr		= reg->reg_Public.Res_Control;
	ioreq	= epr->IORequests[0];
	sd		= epr->SetupData;

	/*
	** Get Control EndPoint size
	*/

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY2 -- Get Control EndPoint Size\n##\n\n" );

	if ( ! __Get_EndPoint_Size( epr, fn, ioreq, sd, usbbase ))
	{
		USBERROR( "Error: Get Control EndPoint size" );
		goto bailout;
	}

	/*
	** Get Full Device Descriptor
	*/

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY3 -- Get Full Device Descriptor\n##\n\n" );

	if ( ! __Get_Device_Desc( fn, ioreq, sd, usbbase ))
	{
		USBERROR( "Error: Get Full Device Descriptor" );
		goto bailout;
	}

	USBDEBUG( "VendorID..: $%04lx", LE_SWAP16( fn->fkt_DeviceDescriptor->VendorID ));
	USBDEBUG( "ProdictID.: $%04lx", LE_SWAP16( fn->fkt_DeviceDescriptor->DeviceID ));
	USBDEBUG( "Class.....: $%02lx", fn->fkt_DeviceDescriptor->DeviceClass );
	USBDEBUG( "SubClass..: $%02lx", fn->fkt_DeviceDescriptor->DeviceSubClass );
	USBDEBUG( "Protocol..: $%02lx", fn->fkt_DeviceDescriptor->DeviceProtocol );

	/*
	** Get Device Status
	*/

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY4 -- Get Device Status\n##\n\n" );

	if ( ! __Get_Device_Stat( fn, ioreq, sd, usbbase ))
	{
		USBERROR( "Error: Get Device Status" );
		goto bailout;
	}

	fn->fkt_Class		= fn->fkt_DeviceDescriptor->DeviceClass;
	fn->fkt_SubClass	= fn->fkt_DeviceDescriptor->DeviceSubClass;
	fn->fkt_Protocol	= fn->fkt_DeviceDescriptor->DeviceProtocol;

	/*
	** Get Config Descriptor
	*/

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY5 -- Get Config Desc Size\n##\n\n" );

	if ( ! __Get_Config_Size( fn, ioreq, sd, usbbase ))
	{
		USBERROR( "Error: Get Config Desc Size" );
		goto bailout;
	}

	/*
	** Get Full Config Descriptor
	*/

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY6 -- Get Full Config Descriptor\n##\n\n" );

	if ( ! __Get_Config_Desc( fn, ioreq, sd, usbbase ))
	{
		USBERROR( "Error: Get Full Config Desc" );
		goto bailout;
	}

	/*
	** Parse Config Descriptor we got
	*/

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY7 -- Parse Config Desc\n##\n\n" );

	// Build full Descriptor setup
	if ( ! CONFIG_PARSE(fn))
	{
		USBERROR( "Error: Parsing Config failed" );
		goto bailout;
	}

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY8 -- Set Config\n##\n\n" );

	// Get Config Nr from First Config Desc
	// this looks hacky .. find a better way or atleast check if its a cn-desc
	U32 cfgval = ((struct USB3_Config_Desc *)fn->fkt_Config_Desc_Buf)->ConfigurationValue;

	// Set Config
	if ( ! CONFIG_SET( fn, (PTR) ioreq, cfgval ))
	{
		USBERROR( "Error: Set Config failed" );
		goto bailout;
	}

	/*
	** Get HUB Descriptor Buffer
	*/

	if ( fn->fkt_Class == USBCLASS_HUB )
	{
		fn->fkt_HUBDescriptor = MEM_ALLOCIOBUFFER( 256, TRUE );

		if ( ! fn->fkt_HUBDescriptor )
		{
			USBERROR( "Error: Allocating memory" );
			goto bailout;
		}
	}

	/*
	** Get Language Descriptor
	*/

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## YAY9 -- Get Strings\n##\n\n" );

	if ( ! __Get_Strings( fn, ioreq, sd, usbbase ))
	{
		USBERROR( "Error: Set Config failed" );
		goto bailout;
	}

	fn->fkt_Initialized = TRUE;

	USBDEBUG( "__myMain (Init)          : Done" );

bailout:

	// --

	USBDEBUG( "__myMain (Init)          : Unregister %p", reg );

	REGISTER_UNREGISTER( reg );

	// -- Exit

	USBDEBUG( "__myMain (Init)          : Leave" );

	return;
}

// --
