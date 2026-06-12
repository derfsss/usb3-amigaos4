
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

SEC_CODE S32 __IO_Do( struct USBBase *usbbase, PTR ioreq, STR file UNUSED )

#else

SEC_CODE S32 __IO_Do( struct USBBase *usbbase, PTR ioreq )

#endif

{
struct ExecIFace *IExec;
S32 errcode;

	TASK_NAME_ENTER( "__IO_Do" );

	#ifdef DO_PANIC

	if (( ! usbbase ) || ( ! ioreq ))
	{
		USBPANIC( "__IO_Do : NULL Pointer" );
	}

	if ( IS_USB_IOREQ(ioreq) ) 
	{
		IOREQUEST_VALID(ioreq);

		if ( usbbase->usb_LockSemaphore.us_Semaphore.ss_Owner == usbbase->usb_IExec->FindTask(NULL) )
		{
			USBPANIC( "__IO_Do : You must NOT hold usb_LockSemaphore for this call : (%s)", (file)?file:"<NULL>" );
		}
	}

	#endif

	IExec = usbbase->usb_IExec;
	IExec->DoIO(ioreq);

	errcode = ((struct IORequest *)ioreq)->io_Error;

	TASK_NAME_LEAVE();
	return( errcode );
}

// --
