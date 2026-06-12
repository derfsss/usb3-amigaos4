
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 OpenTimer( struct USBBase *usbbase )
{
struct USB3_MsgPort mp;
struct ExecIFace *IExec;
S32 retval;

	retval = FALSE;

	IExec = usbbase->usb_IExec;

	if ( ! MSGPORT_INIT( & mp ))
	{
		USBERROR( "Error creating MsgPort" );
		goto bailout;
	}

	usbbase->usb_TimeRequest = (PTR) IExec->AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_Size,		sizeof( struct TimeRequest ),
		ASOIOR_ReplyPort,   & mp.ump_MsgPort,
		TAG_END
	);

	if ( ! usbbase->usb_TimeRequest )
	{
		USBERROR( "Error creating Timer IORequest" );
		goto bailout;
	}

	if ( IExec->OpenDevice( "timer.device", 0, (PTR) usbbase->usb_TimeRequest, UNIT_MICROHZ ))
	{
		USBERROR( "Error opening Timer Device" );
		goto bailout;
	}

	usbbase->usb_TimeRequest->Request.io_Command = TR_ADDREQUEST;
	usbbase->usb_TimeRequest->Time.Seconds = 0;
	usbbase->usb_TimeRequest->Time.Microseconds = 1;
	IO_DO( usbbase->usb_TimeRequest );

	if ( usbbase->usb_TimeRequest->Request.io_Error )
	{
		USBERROR( "Timer returned (%ld)", usbbase->usb_TimeRequest->Request.io_Error );
		goto bailout;
	}

	retval = TRUE;

bailout:

	if (( ! retval ) && ( usbbase->usb_TimeRequest ))
	{
		IExec->FreeSysObject( ASOT_IOREQUEST, usbbase->usb_TimeRequest );
		usbbase->usb_TimeRequest = NULL;
	}

	MSGPORT_FREE( & mp );

	return( retval );
}

// --

SEC_CODE static S32 OpenInput( struct USBBase *usbbase )
{
struct ExecIFace *IExec;
int retval;

	retval = FALSE;

	IExec = usbbase->usb_IExec;

	usbbase->usb_InputIORequest = (PTR) IExec->AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_Size,		sizeof( struct IOStdReq ),
		ASOIOR_ReplyPort,	-1,
		TAG_END
	);

	if ( ! usbbase->usb_InputIORequest )
	{
		USBERROR( "Error creating Input IORequest" );
		goto bailout;
	}

	if ( IExec->OpenDevice( "input.device", 0, (PTR) usbbase->usb_InputIORequest, 0 ))
	{
		USBERROR( "Error opening Input Device" );
		goto bailout;
	}

	usbbase->usb_InputBase = usbbase->usb_InputIORequest->io_Device;

	retval = TRUE;

bailout:

	return( retval );
}

// --

SEC_CODE USED PTR ROMInit( PTR Dummy UNUSED, PTR SegList, struct ExecBase *mySysBase )
{
struct ExpansionIFace *IExpansion;
struct ExecIFace *IExec;
struct USBBase *usbbase;
PTR node;

	usbbase	= NULL;

//	IExec = (PTR)(*(struct ExecBase **)4)->MainInterface;
	IExec = (PTR) mySysBase->MainInterface;
//	IExec->DebugPrintF( "usb3 ROM Init\n" );

	// Make sure we havent started
	if ( IExec->FindName( & mySysBase->DeviceList, "usb3.device" ))
	{
//		IExec->DebugPrintF( "11\n" );
		goto bailout;
	}

	extern const PTR devInterfaces[];
	usbbase = (PTR) IExec->CreateLibraryTags(
		CLT_DataSize, sizeof( struct USBBase ),
		CLT_Interfaces,	devInterfaces,
		CLT_NoLegacyIFace, TRUE,
		TAG_END
	);
	
	if ( ! usbbase )
	{
//		IExec->DebugPrintF( "22\n" );
		goto bailout;
	}

	usbbase->usb_Library.lib_Node.ln_Type	= NT_DEVICE;
	usbbase->usb_Library.lib_Node.ln_Pri	= 0;
	usbbase->usb_Library.lib_Node.ln_Name	= (STR) "usb3.device",
	usbbase->usb_Library.lib_Flags			= LIBF_SUMUSED | LIBF_CHANGED;
	usbbase->usb_Library.lib_Version		= VERSION;
	usbbase->usb_Library.lib_Revision		= REVISION;
	usbbase->usb_Library.lib_IdString		= (STR) VSTRING;
	usbbase->usb_SegList					= SegList;
	usbbase->usb_IExec						= IExec;

	// --
	// -- Optional : libs may not be around yet, but lets try
	usbbase->usb_DOSBase		= (PTR) IExec->OpenLibrary( "dos.library", 0 );
	usbbase->usb_IntuitionBase	= (PTR) IExec->OpenLibrary( "intuition.library", 0 );
	usbbase->usb_MounterBase	= (PTR) IExec->OpenLibrary( "mounter.library", 0 );
	usbbase->usb_IDOS			= (PTR) IExec->GetInterface( (PTR) usbbase->usb_DOSBase, "main", 1, NULL );
	usbbase->usb_IIntuition		= (PTR) IExec->GetInterface( (PTR) usbbase->usb_IntuitionBase, "main", 1, NULL );
	usbbase->usb_IMounter		= (PTR) IExec->GetInterface( (PTR) usbbase->usb_MounterBase, "main", 1, NULL );
	usbbase->usb_IMounterPriv	= (PTR) IExec->GetInterface( (PTR) usbbase->usb_MounterBase, "private", 1, NULL );

	// -- Needed :
	usbbase->usb_UtilityBase	= (PTR) IExec->OpenLibrary( "utility.library", 0 );
	usbbase->usb_ExpansionBase	= (PTR) IExec->OpenLibrary( "expansion.library", 0 );
	usbbase->usb_TimerBase		= (PTR) IExec->FindName( & mySysBase->DeviceList, "timer.device" );
	usbbase->usb_IUtility		= (PTR) IExec->GetInterface( (PTR) usbbase->usb_UtilityBase, "main", 1, NULL );
	usbbase->usb_IExpansion		= (PTR) IExec->GetInterface( (PTR) usbbase->usb_ExpansionBase, "main", 1, NULL );
	usbbase->usb_IPCI			= (PTR) IExec->GetInterface( (PTR) usbbase->usb_ExpansionBase, "pci", 1, NULL );
	usbbase->usb_ITimer			= (PTR) IExec->GetInterface( (PTR) usbbase->usb_TimerBase, "main", 1, NULL );
	usbbase->usb_IMMU			= (PTR) IExec->GetInterface( (PTR) mySysBase, "mmu", 1, NULL );
	usbbase->usb_IUSB3			= (PTR) IExec->GetInterface( (PTR) usbbase, "main", 1, NULL );

	IExec->DebugPrintF( "USB3: ExpansionBase=%p IExpansion=%p IPCI=%p\n",
		usbbase->usb_ExpansionBase, usbbase->usb_IExpansion, usbbase->usb_IPCI );
	IExec->DebugPrintF( "USB3: ITimer=%p IMMU=%p IUSB3=%p\n",
		usbbase->usb_ITimer, usbbase->usb_IMMU, usbbase->usb_IUSB3 );

	if (( ! usbbase->usb_IUtility )
	||	( ! usbbase->usb_IExpansion )
	||	( ! usbbase->usb_IPCI )
	||	( ! usbbase->usb_IMMU )
	||	( ! usbbase->usb_ITimer )
	||	( ! usbbase->usb_IUSB3 ))
	{
		IExec->DebugPrintF( "USB3: Interface check FAILED: IUtility=%p IExpansion=%p IPCI=%p IMMU=%p ITimer=%p IUSB3=%p\n",
			usbbase->usb_IUtility, usbbase->usb_IExpansion, usbbase->usb_IPCI,
			usbbase->usb_IMMU, usbbase->usb_ITimer, usbbase->usb_IUSB3 );
		goto bailout;
	}

	// --

	#ifdef DO_PANIC
	SEC_CODE void VARARGS68K __Debug_Panic( struct USBBase *usbbase, STR filename, U32 linenr, STR fmt, ... );
	usbbase->_Panic = __Debug_Panic;
	#endif

	#ifdef DO_ERROR
	SEC_CODE void VARARGS68K __Debug_Error( struct USBBase *usbbase, STR fmt, ... );
	usbbase->_Error = __Debug_Error;
	#endif

	#ifdef DO_INFO
	SEC_CODE void VARARGS68K __Debug_Info( struct USBBase *usbbase, STR fmt, ... );
	usbbase->_Info = __Debug_Info;
	#endif

	#ifdef DO_DEBUG
	SEC_CODE void VARARGS68K __Debug_Debug( struct USBBase *usbbase, STR fmt, ... );
	usbbase->_Debug = __Debug_Debug;
	#endif

	// --

	#ifdef DO_DEBUG
	usbbase->usb_IExec->DebugPrintF( "\n" );
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBFunction .......... : %2ld, USB3_FunctionNode ..... : %5ld\n", 
		MEMID_USBFunction, sizeof( struct RealFunctionNode ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBEndPoint .......... : %2ld, USB3_EndPointNode ..... : %5ld\n", 
		MEMID_USBEndPoint, sizeof( struct USB3_EndPointNode ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBEndPointResource .. : %2ld, RealEndPointResource .. : %5ld\n", 
		MEMID_USBEndPointResource, sizeof( struct RealEndPointResource ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBIORequest ......... : %2ld, RealRequest ........... : %5ld\n", 
		MEMID_USBIORequest, sizeof( struct RealRequest ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBRegister .......... : %2ld, RealRegister .......... : %5ld\n", 
		MEMID_USBRegister, sizeof( struct RealRegister ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBInterfaceGroup .... : %2ld, USB3_InterfaceGroup ... : %5ld\n", 
		MEMID_USBInterfaceGroup, sizeof( struct USB3_InterfaceGroup ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBInterfaceHeader ... : %2ld, USB3_InterfaceHeader .. : %5ld\n", 
		MEMID_USBInterfaceHeader, sizeof( struct USB3_InterfaceHeader ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBInterfaceNode ..... : %2ld, USB3_InterfaceNode .... : %5ld\n", 
		MEMID_USBInterfaceNode, sizeof( struct USB3_InterfaceNode ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBConfig ............ : %2ld, USB3_ConfigNode ....... : %5ld\n", 
		MEMID_USBConfig, sizeof( struct USB3_ConfigNode ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBDriver ............ : %2ld, USB3_DriverNode ....... : %5ld\n", 
		MEMID_USBDriver, sizeof( struct USB3_DriverNode ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBTaskMsg ........... : %2ld, USB3_TaskMsg .......... : %5ld\n", 
		MEMID_USBTaskMsg, sizeof( struct USB3_TaskMsg ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBHCD ............... : %2ld, USB3_HCDNode .......... : %5ld\n", 
		MEMID_USBHCD, sizeof( struct USB3_HCDNode ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBTaskNode .......... : %2ld, USB3_TaskNode ......... : %5ld\n", 
		MEMID_USBTask, sizeof( struct USB3_TaskNode ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_USBSetupData ......... : %2ld, RealSetupData ......... : %5ld\n", 
		MEMID_USBSetupData, sizeof( struct RealSetupData ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_HCD_4k ............... : %2ld, 4k Buffer ............. : %5ld\n", 
		MEMID_HCD_4k, 1024 * 4 );
	usbbase->usb_IExec->DebugPrintF( "MEMID_HCD_8k ............... : %2ld, 4k 8uffer ............. : %5ld\n", 
		MEMID_HCD_8k, 1024 * 8 );
	usbbase->usb_IExec->DebugPrintF( "MEMID_HCD_20k .............. : %2ld, 20k Buffer ............ : %5ld\n", 
		MEMID_HCD_20k, 1024 * 20 );
	usbbase->usb_IExec->DebugPrintF( "MEMID_EHCI_QH .............. : %2ld, EHCI_QH ............... : %5ld\n", 
		MEMID_EHCI_QH, sizeof( struct EHCI_QH ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_EHCI_TD .............. : %2ld, EHCI_TD ............... : %5ld\n", 
		MEMID_EHCI_TD, sizeof( struct EHCI_TD ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_OHCI_ED .............. : %2ld, OHCI_ED ............... : %5ld\n", 
		MEMID_OHCI_ED, sizeof( struct OHCI_ED ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_OHCI_TD .............. : %2ld, OHCI_TD ............... : %5ld\n", 
		MEMID_OHCI_TD, sizeof( struct OHCI_TD ));
	usbbase->usb_IExec->DebugPrintF( "MEMID_OHCI_HCCA ............ : %2ld, OHCI_HCCA ............. : %5ld\n", 
		MEMID_OHCI_HCCA, sizeof( struct OHCI_HCCA ));
		
	usbbase->usb_IExec->DebugPrintF( "\n" );
	#endif

	// --
	// Init Device Functions
	// eg.  usbbase->_Config_Alloc	= __Config_Alloc;
	// eg.  usbbase->_Config_Free	= __Config_Free;

	#define USB3_CREATEFUNC(ret_type, name, ...) usbbase->_##name = __##name;
	#include "usb3_Protos.h"
	#undef USB3_CREATEFUNC

	// --
	
	SEMAPHORE_INIT( & usbbase->usb_Bind_Semaphore );
	ASYNC_INIT( & usbbase->usb_HCDASync, NULL );

	usbbase->usb_CPU_CacheSize 			= 32;
	usbbase->usb_CPU_PageSize 			= 4096;
	usbbase->usb_CPU_Type				= CPUTYPE_UNKNOWN;

	IExpansion = usbbase->usb_IExpansion;
	IExpansion->GetMachineInfoTags(
		GMIT_Machine, 					& usbbase->usb_MachineType,
		TAG_END
	);

	IExec->GetCPUInfoTags(
		GCIT_CacheLineSize,				& usbbase->usb_CPU_CacheSize,
		GCIT_CPUPageSize,				& usbbase->usb_CPU_PageSize,
		GCIT_Model,						& usbbase->usb_CPU_Type,
		TAG_END
	);

	// --

	USBDEBUG( "Memory Setup" );

	if ( ! MEMORY_SETUP() )
	{
		USBERROR( "Error: Memory Setup failed" );
		goto bailout;
	}

	USBDEBUG( "Open Timer" );

	if ( ! OpenTimer( usbbase ))
	{
		USBERROR( "Error: Opening timer" );
		goto bailout;
	}

	USBDEBUG( "Open Input" );

	if ( ! OpenInput( usbbase ))
	{
		USBERROR( "Error: Opening input" );
		goto bailout;
	}

	USBDEBUG( "Find Controllers" );

	if ( ! HCD_CONTROLLERS_FIND() )
	{
		// Hmm keep output
		IExec->DebugPrintF( "USB3: No Controllers found\n" );
		goto bailout;
	}

	USBDEBUG( "Start Master" );

	if ( ! MASTER_STARTUP() )
	{
		USBERROR( "Failed to start master" );
		goto bailout;
	}

//	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )
	#if defined( DO_PANIC )

	if ( ! usbbase->usb_Master_MsgPort )
	{
		USBPANIC( "ROMInit : Master MsgPort NULL Pointer" );
	}

	#endif

	// --
	#ifdef DRV_HUB

	SEC_CODE void HUB_Entry( void );
	node = FDRIVER_CREATETAGS(
		USB3Tag_FDriver_Title,			"HUB",
		USB3Tag_FDriver_Type,			USB3Val_FDriver_Function,
		USB3Tag_FDriver_Entry,			HUB_Entry,
//		USB3Tag_FDriver_Promote,		HUB_Promote,
		USB3Tag_FDriver_Priority,		5,
		USB3Tag_FDriver_Class,			USBCLASS_HUB,
//		USB3Tag_FDriver_SubClass,		0,
		TAG_END
	);

	if ( ! node )
	{
		USBERROR( "Error adding HUB Class" );
		goto bailout;
	}

	#endif
	// --
	#ifdef DRV_HID

	SEC_CODE void HID_Entry( void );
	node = FDRIVER_CREATETAGS(
		USB3Tag_FDriver_Title,			"HID",
		USB3Tag_FDriver_Type,			USB3Val_FDriver_Interface,
		USB3Tag_FDriver_Entry,			HID_Entry,
//		USB3Tag_FDriver_Promote,		HUB_Promote,
		USB3Tag_FDriver_Priority,		15,
		USB3Tag_FDriver_Class,			USBCLASS_HID,
//		USB3Tag_FDriver_SubClass,		0,
		TAG_END
	);

	if ( ! node )
	{
		USBERROR( "Error adding HID Class" );
		goto bailout;
	}

	#endif
	// --
	#ifdef DRV_MSD

	SEC_CODE void MSD_Entry( void );
	node = FDRIVER_CREATETAGS(
		USB3Tag_FDriver_Title,			"MSD",
		USB3Tag_FDriver_Type,			USB3Val_FDriver_Interface,
		USB3Tag_FDriver_Entry,			MSD_Entry,
//		USB3Tag_FDriver_Promote,		msddisk_Promote,
		USB3Tag_FDriver_Priority,		0,
		USB3Tag_FDriver_Class,			USBCLASS_Mass_Storage,
//		USB3Tag_FDriver_Hotkey,			"CTRL+LALT+M",
		TAG_END
	);

	if ( ! node )
	{
		USBERROR( "Error adding MSD Class" );
		goto bailout;
	}

	#endif
	// --

	USBDEBUG( "__ROMInit : 1111 :" );

	if ( ! HCD_CONTROLLERS_START() )
	{
		USBERROR( "Error starting HCD Task" );
		goto bailout;
	}

	USBDEBUG( "__ROMInit : 1133 :" );

	// --

//	IExec->DebugPrintF( "Add Device\n" );

	IExec->AddDevice( (PTR) usbbase );

//	USBERROR( "Rock'n Roll" );
	IExec->DebugPrintF( "USB3: Rock'n Roll\n" );

//	IExec->Wait(0);

	return( NULL );

	// --
	// -- Shutdown free / resources

bailout:

	USBERROR( "USB Failed .. shutting down" );

	ROMExit( usbbase, __FILE__ );

	USBERROR( "USB Failed .. deleting USB Device" );

	// it have not been added
	IExec->DeleteLibrary( (PTR) usbbase );
	usbbase = NULL;

//	USBERROR( "USB Failed .. exit" );

	return( NULL );
}

// --
