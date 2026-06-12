
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

SEC_CODE struct USB3_HCDNode *__HCD_Alloc( struct USBBase *usbbase, U32 hcdtype, STR file UNUSED )

#else

SEC_CODE struct USB3_HCDNode *__HCD_Alloc( struct USBBase *usbbase, U32 hcdtype )

#endif

{
struct USB3_HCDNode *hn;
S32 error;

	TASK_NAME_ENTER( "HCD : __HCD_Alloc" );

	error = TRUE;

	// --

	hn = MEMORY_ALLOC( MEMID_USBHCD, TRUE );

	if ( ! hn )
	{
		USBERROR( "__HCD_Alloc : Error allocating memory" );
		goto bailout;
	}

	hn->hn_StructID		= ID_USB3_HN;
	hn->hn_NotifyID		= MISC_NEWNOTIFYID();
	hn->hn_HCDType		= hcdtype;
	hn->hn_USBBase		= usbbase;

	// --

	error = FALSE;

bailout:

	if (( hn ) && ( error ))
	{
		HCD_FREE( hn );
		hn = NULL;
	}

	USBDEBUG( "__HCD_Alloc              : HN    %p : (new) : Type %ld", hn, hcdtype );

	TASK_NAME_LEAVE();

	return( hn );
}

// --
