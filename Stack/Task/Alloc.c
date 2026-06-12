
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

SEC_CODE struct USB2_TaskNode *__Task_Alloc( struct USBBase *usbbase, struct USB2_ASync *as, STR file UNUSED )

#else

SEC_CODE struct USB2_TaskNode *__Task_Alloc( struct USBBase *usbbase, struct USB2_ASync *as )

#endif

{
struct USB2_TaskNode *tn;
S32 error;

	USBDEBUG( "__Task_Alloc             : (%s)", (file)?file:"<NULL>" );

	error = TRUE;

	// --

	tn = MEMORY_ALLOC( MEMID_USBTask, TRUE );

	if ( ! tn )
	{
		USBERROR( "__Task_Alloc : Error allocating memory" );
		goto bailout;
	}

	tn->tn_StructID = ID_USB2_TN;

	// --

	if ( as )
	{
		ASYNC_ADD(as);
		tn->tn_ASync = as;
	}

	// --

	error = FALSE;

bailout:

	if ((tn) && ( error ))
	{
		TASK_FREE(tn);
		tn = NULL;
	}

	USBDEBUG( "__Task_Alloc             : AS %p : TN %p : (new)", as, tn );

	return( tn );
}

// --
