
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

SEC_CODE void __Task_FreeSignal( struct USBBase *usbbase, struct USB3_Signal *sig, STR file UNUSED )

#else

SEC_CODE void __Task_FreeSignal( struct USBBase *usbbase, struct USB3_Signal *sig )

#endif

{
struct ExecIFace *IExec;

	if (( sig == NULL )
	||	( sig->sig_StructID == 0 ) 
	||	( sig->sig_StructID == ID_USB3_FREED ))
	{
		return;
	}

	#ifdef DO_PANIC

	if ( sig->sig_StructID != ID_USB3_SIG )
	{
		USBPANIC( "Task_FreeSignal : 1 : Invalid ID" );
	}

	#endif

//	usbbase->usb_IExec->DebugPrintF( "__Task_FreeSignal : Nr %ld : (%s)\n", (S32) sig->sig_Signal_Nr, (file)?file:"<NULL>" );

	if ( sig->sig_Signal_Nr >= 0 )
	{
		IExec = usbbase->usb_IExec;
		IExec->FreeSignal( sig->sig_Signal_Nr );
	}

	sig->sig_StructID		= ID_USB3_FREED;
	sig->sig_Signal_Nr		= -1;
	sig->sig_Signal_Mask	= 0;
}

// --
