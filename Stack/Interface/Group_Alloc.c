
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

SEC_CODE struct USB2_InterfaceGroup *__Interface_AllocGroup(
	struct USBBase *usbbase,
	struct USB2_ConfigNode *cn,
	STR file UNUSED )

#else

SEC_CODE struct USB2_InterfaceGroup *__Interface_AllocGroup(
	struct USBBase *usbbase,
	struct USB2_ConfigNode *cn )

#endif

{
struct USB2_InterfaceGroup *ig;
struct RealFunctionNode *fn;
S32 error;

	USBDEBUG( "__Interface_AllocGroup   : (%s)", (file)?file:"<NULL>" );

	error = TRUE;

	// --

	ig = MEMORY_ALLOC( MEMID_USBInterfaceGroup, TRUE );

	if ( ! ig )
	{
		USBERROR( "__Interface_AllocGroup : Error allocating memory" );
		goto bailout;
	}

	ig->ig_StructID = ID_USB2_IFCG;

	// --

	if ( CONFIG_LOCK(cn) != LSTAT_Okay )
	{
		USBERROR( "__Interface_AllocGroup : Error locking Config" );
		goto bailout;
	}

	ig->ig_Config = cn;

	if (( cn->cfg_Detach ) || ( cn->cfg_FreeMe ))
	{
		USBERROR( "__Interface_AllocGroup : Error Config Detached" );
		goto bailout;
	}

	// --

	fn = cn->cfg_Function;

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		USBERROR( "__Interface_AllocGroup : Error locking Function" );
		goto bailout;
	}

	ig->ig_Function = fn;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBERROR( "__Interface_AllocGroup : Error Funtion Detached" );
		goto bailout;
	}

	// --

	error = FALSE;

bailout:

	if ((ig) && ( error ))
	{
		INTERFACE_FREEGROUP(ig);
		ig = NULL;
	}

	USBDEBUG( "__Interface_AllocGroup   : IG    %p : (new)", ig );

	return( ig );
}

// --
