
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

SEC_CODE struct USB2_DriverNode *__Driver_Alloc( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as UNUSED, STR file UNUSED )

#else

SEC_CODE struct USB2_DriverNode *__Driver_Alloc( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as UNUSED )

#endif

{
struct USB2_DriverNode *dn;
struct USB2_HCDNode *hn;
S32 error;

//	USBDEBUG( "__Driver_Alloc           : FN    %p : ", fn );

	error = TRUE;

	// --

	dn = MEMORY_ALLOC( MEMID_USBDriver, TRUE );

	if ( ! dn )
	{
		USBERROR( "__Driver_Alloc : Error allocating memory" );
		goto bailout;
	}

	dn->dn_StructID = ID_USB2_DN;

	// --

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		USBERROR( "__Driver_Alloc : Error invalid Function (%p)", fn );
		goto bailout;
	}

	dn->dn_Function = fn;

	// --

	hn = fn->fkt_HCD;

	if ( HCD_LOCK( hn ) != LSTAT_Okay )
	{
		USBERROR( "__Driver_Alloc : Error invalid HCD (%p)", hn );
		goto bailout;
	}

	dn->dn_HCD = hn;

	// --

	error = FALSE;

bailout:

	if (( dn ) && ( error ))
	{
		DRIVER_FREE( dn );
		dn = NULL;
	}

	USBDEBUG( "__Driver_Alloc           : DN    %p : (new)", dn );

	return( dn );
}

// --
