
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __SetupData_Free( struct USBBase *usbbase, PTR mem )
{
struct RealSetupData *sd;

	if ( mem )
	{
		sd = mem;

		USBDEBUG( "__SetupData_Free         : SD    %p, Phy $%08lx", sd, sd->rsd_Phy );

		if ( sd->rsd_StructID == ID_USB3_SD )
		{
			MEMORY_FREE( MEMID_USBSetupData, sd, sd->rsd_Phy );
		}
		else
		{
			USBDEBUG( "__SetupData_Free         : SD Invalid StructID ($%08lx)", sd->rsd_StructID );
		}
	}
	else
	{
		USBDEBUG( "__SetupData_Free         : SD    %p", mem );
	}
}

// --
