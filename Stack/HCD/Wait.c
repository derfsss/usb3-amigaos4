
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

#include "HCD.h"

// --
// Delay/Wait function for HCD tasks only
// so I can avoid the MicroDelay that is a Busywait function

SEC_CODE void __HCD_Wait_ms( struct USBBase *usbbase, struct USB3_HCDNode *hn, U32 ms )
{
struct TimeRequest *req;

//	USBDEBUG( "__HCD_Wait_ms : Enter" );
	TASK_NAME_ENTER( "HCD : __HCD_Wait_ms" );

	#ifdef DO_DEBUG

	// Make sure only HCDs use this timer

	if ( hn->hn_Delay_InUse )
	{
		USBPANIC( "__HCD_Wait_ms : Error Delay in use" );		
	}

	hn->hn_Delay_InUse = TRUE;

	#endif

	// --
	// Timer should already be initialized
	// only update the time duration

	req = & hn->hn_Delay_IOReq;

	req->Time.Microseconds = ( ms % 1000 ) * 1000;
	req->Time.Seconds = ( ms / 1000 );

	IO_DO( req );

	// --

	#ifdef DO_DEBUG

	hn->hn_Delay_InUse = FALSE;

	#endif

//	USBDEBUG( "__HCD_Wait_ms : Leave" );

	TASK_NAME_LEAVE();
}

// --
