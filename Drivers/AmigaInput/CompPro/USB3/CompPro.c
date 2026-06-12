
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "CompPro.h"

// --

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS4) this was usually done by
 * moveq #0,d0
 * rts
 *
 */

int32 _start( STRPTR argstring UNUSED, int32 arglen UNUSED, APTR SysBase UNUSED )
{
	/* If you feel like it, open DOS and print something to the user */

	return( RETURN_OK );
}

// --
