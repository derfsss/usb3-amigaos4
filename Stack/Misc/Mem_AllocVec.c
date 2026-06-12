
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Mem_AllocVec( struct USBBase *usbbase UNUSED, U32 size, S32 clr )
{
struct ExecIFace *IExec;
PTR mem;

	if ( size )
	{
		IExec = usbbase->usb_IExec;

		mem = IExec->AllocVecTags( size,
			( clr ) ? AVT_Clear : TAG_IGNORE, 0,
			TAG_END
		);

//		USBDEBUG( "AllocVec Mem             : Mem %p : Size %ld : Clear %ld", mem, size, clr );
	}
	else
	{
		mem = NULL;
	}

	return( mem );
}

// --
