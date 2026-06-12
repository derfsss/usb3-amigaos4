
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#include "Parse_Parse.c"
#include "Parse_Merge.c"
#include "Parse_Display.c"

// --

SEC_CODE S32 __Config_Parse( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
S32 retval;

	retval = FALSE;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBDEBUG( "__Config_Parse : Error Funtion Detached" );
		goto bailout;
	}

	if ( ! __Parse( usbbase, fn ))
	{
		USBERROR( "__Config_Parse : Error parsing Config" );
		goto bailout;
	}

	if ( ! __Merge( usbbase, fn ))
	{
		USBERROR( "__Config_Parse : Error merging Config" );
		goto bailout;
	}

//	#ifdef DO_INFO
//	__Display( usbbase, fn );
//	#endif

	retval = TRUE;

bailout:

	return( retval );
}

// --
