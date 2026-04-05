
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"
#include "XHCI.h"

// --

SEC_CODE U32 XHCI_Handler_Reset( struct ExceptionContext *Context UNUSED, struct ExecBase *SysBase UNUSED, PTR userData UNUSED )
{
	return( 0 );
}

// --
