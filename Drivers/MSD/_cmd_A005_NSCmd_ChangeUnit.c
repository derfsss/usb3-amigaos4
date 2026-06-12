
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --
#if 1

SEC_CODE S32 MSD_Cmd_A005_NSCmd_ChangeUnit( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq )
{
struct MSDBase *msdbase;
struct MSDDisk *msddisk;
U32 old_unit_nr;
U32 new_unit_nr;
S32 reply;

	TASK_NAME_ENTER( "MSD : NSCmd_ChangeUnit" );
	USBERROR( "MSD : _cmd_A005_NSCmd_ChangeUnit" );

	reply = TRUE;

	msdbase = usbbase->usb_MSDDevice;
	msddisk = (PTR) ioreq->io_Unit;

	SEMAPHORE_OBTAIN( & msdbase->msdbase_MSDDisk_Semaphore );

	if ( ioreq->io_Offset == -1U )
	{
		// Find a (new) free slot

		for( new_unit_nr=0 ; new_unit_nr<MAX_MSD_UNITS ; new_unit_nr++ )
		{
			if ( ! msdbase->msdbase_MSDDisk_Units[ new_unit_nr ] )
			{
				break;
			}
		}
	}
	else
	{
		// User requested a specific slot

		new_unit_nr = ioreq->io_Offset;
	}

	old_unit_nr = msddisk->msddisk_UnitNr;

	if (( old_unit_nr >= MAX_MSD_UNITS )	// Valid old Unit Nr
	||	( new_unit_nr >= MAX_MSD_UNITS ))	// Check if we found a New
	{
		USBERROR( "MSD : NSCmd_ChangeUnit : No free Unit slots" );
		ioreq->io_Actual = -1;
		ioreq->io_Error = IOERR_UNITBUSY;	// Not possible, or we refuse
		goto bailout;
	}

	if ( new_unit_nr == old_unit_nr )		// Check if we moved
	{
		USBERROR( "MSD : NSCmd_ChangeUnit : Same UnitNr no move done" );
		ioreq->io_Actual = new_unit_nr;
		ioreq->io_Error = 0;
		goto bailout;
	}

	// -- Swap Unit Nr

	USBERROR( "MSD : NSCmd_ChangeUnit : Swapping Unit Nr %lu -> %lu", old_unit_nr, new_unit_nr );

	msdbase->msdbase_MSDDisk_Units[ new_unit_nr ] = msddisk;
	msdbase->msdbase_MSDDisk_Units[ old_unit_nr ] = NULL;

	msddisk->msddisk_UnitNr = new_unit_nr;

	ioreq->io_Actual = new_unit_nr;
	ioreq->io_Error = 0;

bailout:

	SEMAPHORE_RELEASE( & msdbase->msdbase_MSDDisk_Semaphore );

	TASK_NAME_LEAVE();
	return( reply );
}

#endif
// --
