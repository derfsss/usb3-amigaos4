
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

SEC_CODE struct USB2_InterfaceHeader *__Interface_AllocHeader(
	struct USBBase *usbbase,
	struct USB2_InterfaceGroup *ig,
	U32 ifcnr,
	STR file UNUSED )

#else

SEC_CODE struct USB2_InterfaceHeader *__Interface_AllocHeader(
	struct USBBase *usbbase,
	struct USB2_InterfaceGroup *ig,
	U32 ifcnr )

#endif

{
struct USB2_InterfaceHeader *ih;
struct RealFunctionNode *fn;
S32 error;

	USBDEBUG( "__Interface_AllocHeader  : (%s)", (file)?file:"<NULL>" );

	error = TRUE;

	// --

	ih = MEMORY_ALLOC( MEMID_USBInterfaceHeader, TRUE );

	if ( ! ih )
	{
		USBERROR( "__Interface_AllocHeader : Error allocating memory" );
		goto bailout;
	}

	ih->ih_StructID = ID_USB2_IFCH;

	// --

	if ( INTERFACE_LOCKGROUP(ig) != LSTAT_Okay )
	{
		USBERROR( "__Interface_AllocHeader : Error locking Group" );
		goto bailout;
	}

	ih->ih_Group = ig;

	if (( ig->ig_Detach ) || ( ig->ig_FreeMe ))
	{
		USBERROR( "__Interface_AllocHeader : Error Interface Group Detached" );
		goto bailout;
	}

	// --

	fn = ig->ig_Function;

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		USBERROR( "__Interface_AllocHeader : Error locking Function" );
		goto bailout;
	}

	ih->ih_Function = fn;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBERROR( "__Interface_AllocHeader : Error Funtion Detached" );
		goto bailout;
	}

	// --

	ih->ih_Number	= ifcnr;
	ih->ih_NotifyID = MISC_NEWNOTIFYID();

	// -- Init Public
	ih->ih_Public.FKT_ID	= fn->fkt_NotifyID;
	ih->ih_Public.IFC_ID	= ih->ih_NotifyID;
	ih->ih_Public.IFC_Num	= ifcnr;

	// --

	error = FALSE;

bailout:

	if ((ih) && ( error ))
	{
		INTERFACE_FREEHEADER(ih);
		ih = NULL;
	}

	USBDEBUG( "__Interface_AllocHeader  : IH    %p : (new)", ih );

	return( ih );
}

// --
