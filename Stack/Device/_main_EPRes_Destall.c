
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

static U32 _main_EPRes_Destall( struct USB3IFace *Self, struct USB3_EPResource *epr )
{
struct USBBase *usbbase;
U32 retval;

	retval = FALSE;
	usbbase = (PTR) Self->Data.LibBase;

	if ( ! epr )
	{
		USBERROR( "_main_EPRes_Destall : NULL Pointer" );
		goto bailout;
	}

	#ifdef DO_DEBUG

	struct RealEndPointResource *real;
	real = (PTR) ( (char *) epr - offsetof( struct RealEndPointResource, epr_Public ));

	if ( ENDPOINTRES_VALID( real ) != VSTAT_Okay )
	{
		USBERROR( "_main_EPRes_Destall : Error invalid EndPoint Resource" );
		goto bailout;
	}

	#endif

	retval = ENDPOINTRES_DESTALL( epr );

bailout:

	return( retval );
}
