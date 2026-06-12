
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

//#include "Bind__Entry.c"
#include "Bind_Check_for_Device.c"
#include "Bind_Check_for_Function.c"
#include "Bind_Check_for_Interfaces.c"

// --

SEC_CODE S32 __Function_Bind( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as )
{
enum FDSTAT stat;
S32 retval;

	usbbase->usb_IExec->DebugPrintF( "USB3: Bind: fn=%p addr=%ld class=%ld cfgActive=%p init=%ld\n",
		fn, (U32) fn->fkt_Address, (U32) fn->fkt_Class, fn->fkt_Config_Active, (U32) fn->fkt_Initialized );

	// --

	#ifdef DO_PANIC

	if ( TASK_FIND() == usbbase->usb_Master_Task.tn_TaskAdr )
	{
		// if run from master task, we will get a dead lock when starting a driver
		USBPANIC( "__Function_Bind          : Do NOT run from Master Task" );
	}

	#endif

	// --

	stat = FDSTAT_Not_Found;

	if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
	{
		// Fast check, if someone owns this device.. 
		// no need to check for any drivers
		if ( ! fn->fkt_Owner )
		{
			SEMAPHORE_OBTAIN( & usbbase->usb_FDriver_Semaphore );

			do
			{
				if ( stat == FDSTAT_Not_Found )
				{
					stat = _Check_for_Devices( usbbase, fn, as );
				}

				if ( stat == FDSTAT_Error )
				{
					USBDEBUG( "__Function_Bind : Checking for Devices failed" );
					break;
				}

				if ( stat == FDSTAT_Not_Found )
				{
					stat = _Check_for_Functions( usbbase, fn, as );
				}

				if ( stat == FDSTAT_Error )
				{
					USBDEBUG( "__Function_Bind : Checking for Functions failed" );
					break;
				}

				if ( stat == FDSTAT_Not_Found )
				{
					stat = _Check_for_Interfaces( usbbase, fn, as );
				}

				if ( stat == FDSTAT_Error )
				{
					USBDEBUG( "__Function_Bind : Checking for Interfaces failed" );
					break;
				}
			}
			while( FALSE );

			SEMAPHORE_RELEASE( & usbbase->usb_FDriver_Semaphore );
		}
		else
		{
			// Allready claimed
//			USBERROR( "Skipping Function (claimed) : FN    %p : Class %ld", fn, fn->fkt_Class );
		}

		FUNCTION_UNLOCK( fn );
	}

	retval = ( stat != FDSTAT_Error ) ? TRUE : FALSE ;

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## __Function_Bind 222 : Bind State %ld\n##\n\n", (S32) retval );

	return( retval );
}

// --
