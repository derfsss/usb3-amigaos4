
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_RODATA static const struct USB2_Device_Desc DefaultDeviceDescriptor =
{
/* Length            */ sizeof( struct USB2_Device_Desc ),
/* DescriptorType    */ DSCTYPE_Device,
/* USB               */ LE_SWAP16( 0x0000 ),
/* DeviceClass       */ 0x00,
/* DeviceSubClass    */ 0x00,
/* DeviceProtocol    */ 0x00,
/* MaxPacketSize0    */ 8,
/* Vendor            */ LE_SWAP16( 0x0000 ),
/* Device            */ LE_SWAP16( 0x0000 ),
/* Device            */ LE_SWAP16( 0x0000 ),
/* Manufacturer      */ 0x00,
/* Device            */ 0x00,
/* SerialNumber      */ 0x00,
/* NumConfigurations */ 0x01
};

SEC_RODATA static const struct USB2_EndPoint_Desc DefaultControl8 =
{
/* Length			*/ 7,
/* DescriptorType	*/ DSCTYPE_EndPoint,
/* EndPointAddress	*/ 0,
/* Attributes		*/ EPATT_Type_Control,
/* MaxPacketSize	*/ LE_SWAP16( 8 ),
/* Interval			*/ 0,
};

SEC_RODATA static const struct USB2_EndPoint_Desc DefaultControl64 =
{
/* Length			*/ 7,
/* DescriptorType	*/ DSCTYPE_EndPoint,
/* EndPointAddress	*/ 0,
/* Attributes		*/ EPATT_Type_Control,
/* MaxPacketSize	*/ LE_SWAP16( 64 ),
/* Interval			*/ 0,
};

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE struct RealFunctionNode *__Function_Alloc( 
	struct USBBase *usbbase, 
	struct USB2_HCDNode *hn, 
	struct USB2_ASync *as, 
	U32 Speed,
	U32 Tier,
	STR file UNUSED )

#else

SEC_CODE struct RealFunctionNode *__Function_Alloc( 
	struct USBBase *usbbase, 
	struct USB2_HCDNode *hn, 
	struct USB2_ASync *as, 
	U32 Speed,
	U32 Tier )

#endif

{
struct USB2_EndPointNode *epn;
struct RealFunctionNode *fn;
S32 error;

	error = TRUE;

	USBDEBUG( "__Function_Alloc         : (%s)", (file)?file:"<NULL>" );

	fn = MEMORY_ALLOC( MEMID_USBFunction, TRUE );

	if ( ! fn )
	{
		USBERROR( "__Function_Alloc : Error allocating memory" );
		goto bailout;
	}

	SEMAPHORE_INIT( & fn->fkt_Semaphore );

	fn->fkt_DriverNode.un_Data = fn;
	fn->fkt_GlobalNode.un_Data = fn;

	fn->fkt_StructID	= ID_USB2_FKT;
	fn->fkt_Tier		= Tier;
	fn->fkt_NotifyID	= MISC_NEWNOTIFYID();
//	fn->fkt_ConfigNr	= -1;
	fn->fkt_ASync		= as;
	fn->fkt_Speed		= Speed;

	// -- Init Public
	fn->fkt_Public.FKT_ID	= fn->fkt_NotifyID;
	fn->fkt_Public.HCD_ID	= hn->hn_NotifyID;

	// --
	ASYNC_ADD(as);

	SEMAPHORE_OBTAIN(	& usbbase->usb_Fkt_Semaphore );
//	NODE_ADDTAIL(		& usbbase->usb_Fkt_Header, & fn->fkt_GlobalNode );
	NODE_ADDHEAD(		& usbbase->usb_Fkt_Header, & fn->fkt_GlobalNode );
	SEMAPHORE_RELEASE(	& usbbase->usb_Fkt_Semaphore );

	// -- 

	if ( HCD_LOCK( hn ) != LSTAT_Okay )
	{
		USBERROR( "__Function_Alloc : Error locking HCD %p", hn );
		goto bailout;
	}

	fn->fkt_HCD = hn;

	// Every Function has aleast the Control EndPoint

	// Set Control endpoints

	if ( fn->fkt_Speed == USBSPEED_High )
	{
		USBINFO( "__Function_Alloc : %ld : High speed", (S32) fn->fkt_Speed );
		epn = ENDPOINT_ALLOC( fn, NULL, (PTR) & DefaultControl64 );
	}
	else
	{
		USBINFO( "__Function_Alloc : %ld : Other speed", (S32) fn->fkt_Speed );
		epn = ENDPOINT_ALLOC( fn, NULL, (PTR) & DefaultControl8 );
	}

	if ( ! epn )
	{
		USBERROR( "__Function_Alloc : Error allocating memory" );
		goto bailout;
	}

	fn->fkt_ControlEndPoint = epn;
	fn->fkt_DeviceDescriptor = MEM_ALLOCIOBUFFER( sizeof( struct USB2_Device_Desc ), FALSE );

	if ( ! fn->fkt_DeviceDescriptor )
	{
		USBERROR( "__Function_Alloc : Error allocating memory" );
		goto bailout;
	}

	MEM_COPY( (PTR) & DefaultDeviceDescriptor, fn->fkt_DeviceDescriptor, sizeof( struct USB2_Device_Desc ));

	// --

	error = FALSE;

bailout:

	if (( fn ) && ( error ))
	{
		FUNCTION_FREE(fn);
		fn = NULL;
	}
//	else
//	{
//		usbbase->usb_IExec->DebugPrintF( "FN %p : New\n", fn );
//	}

	USBDEBUG( "__Function_Alloc         : HN    %p : (new) : Tier %ld", hn, Tier );

	return( fn );
}

// --
