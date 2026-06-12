
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

/*
** Running under Disable() and must not fail.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void __Task_ReallocSignal( struct USBBase *usbbase, struct USB3_Signal *sig, STR file UNUSED )

#else

SEC_CODE void __Task_ReallocSignal( struct USBBase *usbbase, struct USB3_Signal *sig )

#endif

{
struct ExecIFace *IExec;
S32 signal;

	#ifdef DO_PANIC

	if ( ! sig )
	{
		USBPANIC( "__Task_ReallocSignal : 1 : NULL Pointer" );
	}

	if (( sig->sig_StructID ) && ( sig->sig_StructID != ID_USB3_SIG ))
	{
		USBPANIC( "__Task_ReallocSignal : 2 : Invalid : %s", file );
	}

	#endif

	IExec	= usbbase->usb_IExec;
	signal	= IExec->AllocSignal( -1 );

	sig->sig_Signal_Nr		= signal;
	sig->sig_Signal_Mask	= 1UL << signal;
}

// --
