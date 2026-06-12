
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __HCD_Addr_Release( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
struct USB3_HCDNode *hn;
U32 adr;

	TASK_NAME_ENTER( "__HCD_Addr_Release" );

	if ( FUNCTION_VALID(fn) != VSTAT_Okay )
	{
		goto bailout;
	}

	adr = fn->fkt_Address;

	if ( ! adr )
	{
		// Not an error
		goto bailout;
	}

	#ifdef DO_DEBUG

	if ( adr >= 128 )
	{
		USBDEBUG( "__HCD_Addr_Release : Invalid Function Address #%lu", adr );
		goto bailout;
	}

	#endif

	hn = fn->fkt_HCD;

	if ( HCD_VALID( hn ) != VSTAT_Okay )
	{
		goto bailout;
	}

	SEMAPHORE_OBTAIN( & usbbase->usb_Addr_Semaphore );

	if ( adr )
	{
		// hmm what to do here.. 0 is roothub
		// 255 is out of range, crash?
		// maybe dont change ?
		fn->fkt_Address = 255;	

		hn->hn_Function_Array[ adr ] = NULL;

		#if 0
		{
			U32 cnt;
			usbbase->usb_IExec->Disable();
			usbbase->usb_IExec->DebugPrintF( "\n## Active Config EndPoints (Free)\nIn : " );
			for( cnt=0 ; cnt<16 ; cnt++ )
			{
				usbbase->usb_IExec->DebugPrintF( "%08lx ", hn->hn_EndPoint_Array[ adr * 32 + cnt ] );
			}
			usbbase->usb_IExec->DebugPrintF( "\nOut: " );
			for( ; cnt<32 ; cnt++ )
			{
				usbbase->usb_IExec->DebugPrintF( "%08lx ", hn->hn_EndPoint_Array[ adr * 32 + cnt ] );
			}
			usbbase->usb_IExec->DebugPrintF( "\n##\n\n" );
			usbbase->usb_IExec->Enable();
		}
		#endif

		// Each Address have 16 in and 16 out EndPoints allocated to it
		MEM_SET( & hn->hn_EndPoint_Array[ adr * 32 ], 0, 32*4 );

		USBDEBUG( "__HCD_Addr_Release       : Address #%lu released", adr );
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Addr_Semaphore );

bailout:

	TASK_NAME_LEAVE();

	return;
}

// --
