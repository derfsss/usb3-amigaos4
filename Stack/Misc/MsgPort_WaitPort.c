
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if 0
void MsgPort_WaitPort( struct USB3_MsgPort *mp )
{
	if ( ! mp )
	{
		return;
	}

	if ( mp->ump_StructID != ID_VNCMPORT )
	{
		printf( "Invalid VNC MsgPort (%p)\n", (PTR) mp );
		return;
	}

	WaitPort( & mp->ump_MsgPort );
}
#endif

// --
