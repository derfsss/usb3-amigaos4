
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

static struct RealRegister *_main_Reg_RegisterList( struct USB3IFace *Self, struct TagItem *taglist )
{
struct RealRegister *reg;
struct USBBase *usbbase;

	usbbase = (PTR) Self->Data.LibBase;
	TASK_NAME_ENTER( "_main_Reg_RegisterList" );

	USBERROR( "USB3 Stack : _main_Reg_RegisterList" );

	reg = REGISTER_REGISTERLIST( taglist );

	TASK_NAME_LEAVE();

	return( reg );
}

// --

static struct RealRegister * VARARGS68K _main_Reg_RegisterTags( struct USB3IFace *Self, ... )
{
struct RealRegister *reg;
struct USBBase *usbbase;
va_list ap;

	usbbase = (PTR) Self->Data.LibBase;
	TASK_NAME_ENTER( "_main_Reg_RegisterTags" );

	USBERROR( "USB3 Stack : _main_Reg_RegisterTags" );

	va_start( ap, Self );

	reg = REGISTER_REGISTERLIST( va_getlinearva( ap, struct TagItem * ));

	va_end( ap );

	TASK_NAME_LEAVE();

	return( reg );
}

// --
