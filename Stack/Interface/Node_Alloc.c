
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

SEC_CODE struct USB2_InterfaceNode *__Interface_AllocNode(
	struct USBBase *usbbase,
	struct USB2_InterfaceHeader *ih,
	STR file UNUSED )

#else

SEC_CODE struct USB2_InterfaceNode *__Interface_AllocNode(
	struct USBBase *usbbase,
	struct USB2_InterfaceHeader *ih )

#endif

{
struct USB2_InterfaceNode *in;
struct RealFunctionNode *fn;
S32 error;

	USBDEBUG( "__Interface_AllocNode    : (%s)", (file)?file:"<NULL>" );

	error = TRUE;

	// --

	in = MEMORY_ALLOC( MEMID_USBInterfaceNode, TRUE );

	if ( ! in )
	{
		USBERROR( "__Interface_AllocNode : Error allocating memory" );
		goto bailout;
	}

	in->in_StructID = ID_USB2_IFCN;

	// --

	if ( INTERFACE_LOCKHEADER(ih) != LSTAT_Okay )
	{
		USBERROR( "__Interface_AllocNode : Error locking Interface Header" );
		goto bailout;
	}

	in->in_Parent = ih;

	if (( ih->ih_Detach ) || ( ih->ih_FreeMe ))
	{
		USBERROR( "__Interface_AllocNode : Error Interface Header Detached" );
		goto bailout;
	}

	// --

	fn = ih->ih_Function;

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		USBERROR( "__Interface_AllocNode : Error locking Function Node" );
		goto bailout;
	}

	in->in_Function = fn;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBERROR( "__Interface_AllocNode : Error Funtion Detached" );
		goto bailout;
	}

	// --

	error = FALSE;

bailout:

	if (( in ) && ( error ))
	{
		INTERFACE_FREENODE( in );
		in = NULL;
	}

	USBDEBUG( "__Interface_AllocNode    : IN    %p : (new)", in );

	return( in );
}

// --
