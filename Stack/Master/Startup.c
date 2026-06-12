
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "Startup_Promote.c"
#include "Startup_Process.c"
#include "Startup_Task.c"
#include "Master.h"

// --

SEC_CODE S32 __Master_Startup( struct USBBase *usbbase )
{
struct intern *in;
S32 retval;

	retval = FALSE;

//	usbbase->usb_IExec->DebugPrintF( "__Master_Startup 1\n" );

	// --

	in = MEM_ALLOCVEC( sizeof( struct intern ), TRUE );

	if ( ! in )
	{
		USBERROR( "__Master_Startup : Error Allocating memory" );
		goto bailout;
	}

	usbbase->usb_Master_Intern = in;

	#ifdef DO_DEBUG
	in->StructID = ID_IN_MASTER;
	#endif

	// --

	if (( ! __myMaster_StartProcess( usbbase )) && ( ! __myMaster_StartTask( usbbase )))
	{
		USBERROR( "__Master_Startup : Error Failed to start Master" );
		goto bailout;
	}

	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

	if ( ! usbbase->usb_Master_Task.tn_TaskAdr )
	{
		USBPANIC( "__Master_Startup : Master Task NULL Pointer" );
	}

	if ( ! usbbase->usb_Master_MsgPort )
	{
		USBPANIC( "__Master_Startup : Master MsgPort NULL Pointer" );
	}

	#endif

	// --

	retval = TRUE;

bailout:

//	usbbase->usb_IExec->DebugPrintF( "__Master_Startup 99 : Retval %ld\n", retval );

	return( retval );
}

// --
