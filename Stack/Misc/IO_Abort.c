
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

SEC_CODE void __IO_Abort( struct USBBase *usbbase, PTR ioreq, STR file UNUSED )

#else

SEC_CODE void __IO_Abort( struct USBBase *usbbase, PTR ioreq )

#endif

{
struct ExecIFace *IExec;

	TASK_NAME_ENTER( "__IO_Abort" );

	USBDEBUG( "__IO_Abort : IOReq %p : (%s)", ioreq, (file)?file:"<NULL>" );

	#ifdef DO_PANIC

	if (( ! usbbase ) || ( ! ioreq ))
	{
		USBPANIC( "__IO_Abort : NULL Pointer" );
	}

	if ( IS_USB_IOREQ(ioreq) ) 
	{
		IOREQUEST_VALID(ioreq);

		if ( usbbase->usb_LockSemaphore.us_Semaphore.ss_Owner == usbbase->usb_IExec->FindTask(NULL) )
		{
			USBPANIC( "__IO_Abort : You must NOT hold usb_LockSemaphore for this call : (%s)", (file)?file:"<NULL>" );
		}
	}

	#endif

	IExec = usbbase->usb_IExec;
	IExec->AbortIO(ioreq);

	TASK_NAME_LEAVE();
}

// --
