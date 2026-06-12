
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

SEC_CODE struct USB2_ConfigNode *__Config_Alloc(
	struct USBBase *usbbase,
	struct RealFunctionNode *fn,
	STR file UNUSED )

#else

SEC_CODE struct USB2_ConfigNode *__Config_Alloc(
	struct USBBase *usbbase,
	struct RealFunctionNode *fn )

#endif

{
struct USB2_ConfigNode *cn;
S32 error;

	error = TRUE;

//	USBDEBUG( "__Config_Alloc           : (%s)", (file)?file:"<NULL>" );

	cn = MEMORY_ALLOC( MEMID_USBConfig, TRUE );

	if ( ! cn )
	{
		USBERROR( "__Config_Alloc : Error allocating memory" );
		goto bailout;
	}

	cn->cfg_StructID = ID_USB2_CFG;

	// --

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		USBERROR( "__Config_Alloc : Error locking Funtion" );
		goto bailout;
	}

	cn->cfg_Function = fn;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBERROR( "__Config_Alloc : Error Funtion Detached" );
		goto bailout;
	}

	// --

	error = FALSE;

bailout:

	if ((cn) && ( error ))
	{
		CONFIG_FREE(cn);
		cn = NULL;
	}

	USBDEBUG( "__Config_Alloc           : CN    %p : (new)", cn );

	return( cn );
}

// --
