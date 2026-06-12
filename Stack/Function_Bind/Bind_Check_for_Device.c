
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 _Start_Device( struct USBBase *usbbase UNUSED, struct RealFunctionNode *fn UNUSED )
{
S32 retval;

	TASK_NAME_ENTER( "_Start_Device" );

	USBDEBUG( "_Start_Device : Implement me" );

	retval = FALSE;

	TASK_NAME_LEAVE();

	return( retval );
}

// --

SEC_CODE static S32 _Check_for_Device( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_FktDriverNode *fdn )
{
struct USB2_DeviceNode *udn;
struct ExecIFace *IExec;
S32 retval;
U16 vid;
U16 pid;

	TASK_NAME_ENTER( "_Check_for_Device" );

//	USBDEBUG( "_Check_for_Device" );

	retval = FALSE;

	IExec = usbbase->usb_IExec;

	vid = LE_SWAP16( fn->fkt_DeviceDescriptor->VendorID  );
	pid = LE_SWAP16( fn->fkt_DeviceDescriptor->DeviceID );
	udn = (PTR) IExec->GetHead( & fdn->fdn_Devices );

	while( udn )
	{
		while( TRUE )
		{
			if ( udn->DeviceID != vid )
			{
				break;
			}

			if ( udn->DeviceID != pid )
			{
				break;
			}

			if ( ! _Start_Device( usbbase, fn ))
			{
				break;
			}

//			USBDEBUG( "Found Bind Device" );

			retval = TRUE;
			break;
		}

		if ( retval )
		{
			break;
		}

		udn = (PTR) IExec->GetSucc( (PTR) udn );
	}

	TASK_NAME_LEAVE();

	return( retval );
}

// --

static enum FDSTAT _Check_for_Devices( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as UNUSED )
{
struct USB2_FktDriverNode *fdn;
enum FDSTAT stat;

//	USBERROR( "_Check_for_Devices " );

	stat = FDSTAT_Not_Found;

	fdn = Header_Head( & usbbase->usb_FDriver_Header );

	while( fdn )
	{
		while( fdn->fdn_Type == USB2Val_FDriver_Device )
		{
			if ( fdn->fdn_Detach )
			{
				break;
			}

			if ( ! _Check_for_Device( usbbase, fn, fdn ))
			{
				break;
			}
	
//			USBDEBUG( "Found Device (possible)" );
			USBERROR( "Found Device (possible)" );

			if ( ! _Start_Device( usbbase, fn ))
			{
				break;
			}

//			USBDEBUG( "Found Device" );
			USBERROR( "Found Device" );

			stat = FDSTAT_Found;
			break;
		}

		if ( stat == FDSTAT_Found )
		{
			break;
		}

		fdn = Node_Next( fdn );
	}

	return( stat );
}

// --
