
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "Init_2_Main.c"

// --

SEC_CODE S32 __Function_Init( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB3_ASync *as )
{
//enum TaskReturn stat;
S32 retval;

	retval = FALSE;

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## __Function_Init 111 :\n##\n\n" );

	if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
	{
		USBDEBUG( "Device Init Done 1       : FN    %p : Locks %2ld : Init %ld", fn, (fn)?fn->fkt_Locks:0, (fn)?fn->fkt_Initialized:-1 );

		if ( ! fn->fkt_Initialized )
		{
			// Init just retreive USB Data about the Device
			// it do not need a ASync struct, as it returns when done
			// hmm but do I need to start another process for that??
			// or can I run it from kickstart/opendevice(ramlib)??
//			stat = 
			TASK_START(
				USB3_WaitForTermination, TRUE,
				TASK_Tag_Func_Main, __myMain,
				TASK_Tag_UserData, fn,
				TASK_Tag_ASync, as,
				TASK_Tag_Type, TASK_Type_FKT,
				TASK_Tag_Name, "Device Init",
				TAG_END	
			);

//			usbbase->usb_IExec->DebugPrintF( "\n##\n## __Function_Init : 000 : Stat $%08lx\n##\n\n", (U32) stat );
		}

		USBDEBUG( "Device Init Done 2 : FN    %p : Locks %2ld : Init %ld", fn, (fn)?fn->fkt_Locks:0, (fn)?fn->fkt_Initialized:-1 );

		if ( fn->fkt_Initialized )
		{
			usbbase->usb_IExec->DebugPrintF( "USB3: Init done: addr=%ld cfgActive=%p class=%ld\n",
				(U32) fn->fkt_Address, fn->fkt_Config_Active, (U32) fn->fkt_Class );
			retval = TRUE;
		}
		else
		{
			USBERROR( "__Function_Init : Stat 222 --- Failed" );
		}

		FUNCTION_UNLOCK( fn );
	}
	else
	{
		USBERROR( "__Function_Init : Function Lock Error" );
	}

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## __Function_Init 333 : Retval %ld\n##\n\n", (S32) retval );

	return( retval );
}

// --
