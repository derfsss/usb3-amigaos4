
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

/* -- Main Interface -- */

static S32 _main_Ifc_Declaim( struct USB2IFace *Self, struct RealRegister *reg, struct USB2_Interface *ifc )
{
struct USB2_InterfaceHeader *ih;
struct USBBase *usbbase;
S32 retval;

	retval = FALSE;

	usbbase = (PTR) Self->Data.LibBase;

	if ( ! ifc )
	{
		USBERROR( "_main_Ifc_Declaim : NULL Pointer" );
		goto bailout;
	}

	ih = (PTR) ( (char *) ifc - offsetof( struct USB2_InterfaceHeader, ih_Public ));

	#ifdef DO_DEBUG

	// Declaim also validates IH, so not needed here
	// but good for early error detection in debug mode
	if ( INTERFACE_VALIDHEADER( ih ) != VSTAT_Okay )
	{
		USBERROR( "main_Ifc_Declaim : Error invalid Interface Pointer" );
		goto bailout;
	}

	// Declaim also validates Reg, so not needed here
	// but good for early error detection in debug mode
	if ( REGISTER_VALID( reg ) != VSTAT_Okay )
	{
		USBERROR( "main_Ifc_Declaim : Error invalid Register Pointer" );
		goto bailout;
	}

	#endif

	INTERFACE_DECLAIMHEADER( reg, ih );

	retval = TRUE;

bailout:

	return( retval );
}

// --
