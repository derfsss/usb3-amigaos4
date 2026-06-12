
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE S32 __Task_SetPri( struct USBBase *usbbase, S32 pri )
{
struct ExecIFace *IExec;
struct Task *Self;
S32 oldpri;

	IExec	= usbbase->usb_IExec;
	Self	= IExec->FindTask( NULL );
	oldpri	= IExec->SetTaskPri( Self, pri );

	return( oldpri );
}

// --

