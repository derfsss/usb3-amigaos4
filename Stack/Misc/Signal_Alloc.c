
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE S32 __Task_AllocSignal( struct USBBase *usbbase, struct USB2_Signal *sig, STR file UNUSED )

#else

SEC_CODE S32 __Task_AllocSignal( struct USBBase *usbbase, struct USB2_Signal *sig )

#endif

{
struct ExecIFace *IExec;
S32 signal;
S32 retval;

	#ifdef DO_PANIC

	if ( ! sig )
	{
		USBPANIC( "Task_AllocSignal : 1 : NULL Pointer" );
	}

// Structure may be on stack
//	if (( sig->sig_StructID != 0 ) && ( sig->sig_StructID != ID_USB2_FREED ))
//	{
//		USBPANIC( "Task_AllocSignal : 2 : IN use" );
//	}

	if ( sig->sig_StructID == ID_USB2_SIG )
	{
		USBPANIC( "Task_AllocSignal : 2 : IN use : %s", file );
	}

	#endif

	IExec = usbbase->usb_IExec;
	signal = IExec->AllocSignal( -1 );

//	usbbase->usb_IExec->DebugPrintF( "Task_AllocSignal : Nr %ld : (%s)\n", (S32) signal, (file)?file:"<NULL>" );

	if ( signal < 0 )
	{
		sig->sig_StructID		= ID_USB2_FREED;
		sig->sig_Signal_Nr		= -1;
		sig->sig_Signal_Mask	= 0;
		retval = FALSE;
	}
	else
	{
		sig->sig_StructID		= ID_USB2_SIG;
		sig->sig_Signal_Nr		= signal;
		sig->sig_Signal_Mask	= 1UL << signal;
		retval = TRUE;
	}

	return( retval );
}

// --
