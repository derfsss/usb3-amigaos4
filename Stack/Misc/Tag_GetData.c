
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Misc_GetTagData( struct USBBase *usbbase, U32 tagid, U32 defvalue, struct TagItem *taglist )
{
struct UtilityIFace *IUtility;
PTR retval;

	IUtility = usbbase->usb_IUtility;
	retval = (PTR) IUtility->GetTagData( tagid, defvalue, taglist );

//	USBDEBUG( 11, "GetTagData(  %08lx [%08lx] ) -> %08lx", tagid, defvalue, retval );

	return( retval );
}

// --
