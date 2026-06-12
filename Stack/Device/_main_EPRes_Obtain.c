
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

static struct USB3_EPResource *_main_EPRes_ObtainList( struct USB3IFace *Self, struct RealRegister *reg, struct TagItem *taglist )
{
struct USBBase *usbbase;
PTR epres;

	usbbase = (PTR) Self->Data.LibBase;
	TASK_NAME_ENTER( "_main_EPRes_ObtainList" );

	USBERROR( "USB3 Stack : _main_EPRes_ObtainList" );

	epres = ENDPOINTRES_OBTAINLIST( reg, taglist );

	TASK_NAME_LEAVE();

	return( epres );
}

// --

static struct USB3_EPResource * VARARGS68K _main_EPRes_ObtainTags( struct USB3IFace *Self, struct RealRegister *reg, ... )
{
struct USBBase *usbbase;
va_list ap;
PTR epres;

	usbbase = (PTR) Self->Data.LibBase;
	TASK_NAME_ENTER( "_main_EPRes_ObtainTags" );

	USBERROR( "USB3 Stack : _main_EPRes_ObtainTags" );

	va_start( ap, reg );

	epres = ENDPOINTRES_OBTAINLIST( reg, va_getlinearva( ap, struct TagItem * ));

	va_end( ap );

	TASK_NAME_LEAVE();

	return( epres );
}

// --
