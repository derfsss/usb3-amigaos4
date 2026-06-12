
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

SEC_CODE S32 MSD_Disk_TestUnitReady( struct USBBase *usbbase, struct MSDDisk *msddisk )
{
struct DoBulkResult dbr;
S32 retval;
U32 tries;
U8 sense[18];
U8 sense_key;

	TASK_NAME_ENTER( "MSD_Disk_TestUnitReady" );
	USBDEBUG( "MSD : MSD_Disk_TestUnitReady" );

	retval = FALSE;

	// --

	for( tries=0 ; tries<30 ; tries++ )
	{
		USBERROR( "MSD_Mount : Calling MSD_SCSI_TestUnitReady : Try #%ld", tries );

		if ( MSD_SCSI_TestUnitReady( usbbase, msddisk, & dbr ))
		{
			USBERROR( "MSD_Mount : Test Unit Ready : Okay" );
			break;
		}

		if ( dbr.csw_Valid )
		{
			/**/ if ( dbr.csw_Status == MSDCSW_Status_CmdFailed )
			{
				USBERROR( "MSD_Mount : TUR : CSW_Cmdfailed" );
				USBERROR( "MSD_Mount : Calling MSD_SCSI_RequestSense" );

				if ( ! MSD_SCSI_RequestSense( usbbase, msddisk, NULL, sense, sizeof( sense )))
				{
					USBERROR( "MSD_Mount : Error Request Sense Failed" );
					MISC_MICRODELAY( 100 * 1000 );
					continue;
				}

				sense_key = sense[2] & 0x0f;

				/**/ if ( sense_key == 0x02 )
				{
					USBERROR( "MSD_Mount : TUR : Sense : Not Ready (2)" );
					// No delay
					continue;
				}
				else if ( sense_key == 0x06 )
				{
					USBERROR( "MSD_Mount : TUR : Sense : Unit Attention (6)" );
					MISC_MICRODELAY( 100 * 1000 );
					continue;
				}
				else
				{
					USBERROR( "MSD_Mount : TUR : Sense : Unknown (%ld)", sense_key );
					MISC_MICRODELAY( 100 * 1000 );
					continue;
				}
			}
			else if ( dbr.csw_Status == MSDCSW_Status_PhaseError )
			{
				USBERROR( "MSD_Mount : TUR : CSW_PhaseError" );
				MISC_MICRODELAY( 100 * 1000 );
				continue;
			}
			else
			{
				USBERROR( "MSD_Mount : TUR : Unknown CSW Error" );
				MISC_MICRODELAY( 100 * 1000 );
				continue;
			}
		}
		else
		{
			USBERROR( "MSD_Mount : TUR : Unknown Error" );
			MISC_MICRODELAY( 100 * 1000 );
			continue;
		}
	}

	if ( tries >= 30 )
	{
		USBERROR( "MSD_Mount : Error Test Unit Failed" );
		goto bailout;
	}

	// --

	USBERROR( "MSD_Mount : MSD_Disk_TestUnitReady okay" );

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();
	return( retval );
}

// --
