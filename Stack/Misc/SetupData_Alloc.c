
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __SetupData_Alloc( struct USBBase *usbbase )
{
struct RealSetupData *sd;

	sd = MEMORY_ALLOC( MEMID_USBSetupData, FALSE );

	if ( sd )
	{
		sd->rsd_Phy = ((struct Mem_FreeNode *)sd)->mfn_Addr;
		sd->rsd_StructID = ID_USB2_SD;

		USBDEBUG( "__SetupData_Alloc        : SD    %p, Phy $%08lx", sd, sd->rsd_Phy );
	}
	else
	{
		USBERROR( "__SetupData_Alloc        : SD    %p : Error", sd );
	}

	return( sd );
}

// --
