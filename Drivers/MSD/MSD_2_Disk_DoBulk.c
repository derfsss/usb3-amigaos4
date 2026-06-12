
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

SEC_CODE S32 MSD_Disk_DoBulk( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkStruct *dbs )
{
struct DoBulkResult *dbr;
struct MSDDevice *msddev;
struct ExecIFace *IExec;
enum PhaseStat stat;
U32 host_Tag;
U32 csw_Tag;
U32 residue;
U32 actual;
S32 retval;
S32 tries;

	TASK_NAME_ENTER( "MSD_Disk_DoBulk" );

	msddev	= msddisk->msddisk_MSDDev;

//	USBERROR( "MSD : MSD_Disk_DoBulk : Lun %ld : CmdLen %ld : Mem %p : MemLen %ld : Flags $%04lx", 
//		msddisk->msddisk_Lun, 
//		dbs->dbs_Command_Length, 
//		dbs->dbs_Data, 
//		dbs->dbs_Data_Length,
//		dbs->dbs_Flags
//	);

	retval	= FALSE;
	IExec	= usbbase->usb_IExec;
	dbr		= dbs->dbs_Result;

	if ( dbs->dbs_Command_Length > 16 )
	{
		#if defined( DO_ERROR ) || defined( DO_INFO ) || defined( DO_DEBUG )
		USBERROR( "Limiting CmdLen to 16 bytes (Len %ld)", dbs->dbs_Command_Length );
		#endif
		dbs->dbs_Command_Length = 16;
	}

	for( tries=0 ; tries<3 ; tries++ )
	{
		// Reset result struct for this attempt
		if ( dbr )
		{
			dbr->csw_DataResidue = 0;
			dbr->csw_DataActual	= 0;
			dbr->csw_Valid		= FALSE;
			dbr->csw_Status		= 0;
			dbr->csw_SigOK		= 0;
			dbr->csw_TagOK		= 0;
		}

		// Zero ID is invalid
		while( ++ msddev->msddev_Bulk_TagID == 0 );

		// --

//		IExec->DebugPrintF( "__phase_command : Calling Function\n" );

		stat = MSD_Bulk_Cmd_Phase( usbbase, msddisk, dbs );

//		IExec->DebugPrintF( "__phase_command : Stat %ld\n", stat );

		if ( stat != PS_Okay )
		{
			IExec->DebugPrintF( "-- Doing another try : Try cnt %ld\n", tries );

			MSD_DoBulk_Recovery( usbbase, msddisk );
			continue;
		}

		// --

		if (( dbs->dbs_Data ) && ( dbs->dbs_Data_Length > 0 ))
		{
//			IExec->DebugPrintF( "MSD_Bulk_Cmd_Data : Calling Function\n" );

			stat =MSD_Bulk_Cmd_Data( usbbase, msddisk, dbs );

//			IExec->DebugPrintF( "MSD_Bulk_Cmd_Data : Stat %ld\n", stat );

			// if error, try and read status
		}

		// --
		
//		IExec->DebugPrintF( "MSD_Bulk_Cmd_Status : Calling Function\n" );

		stat = MSD_Bulk_Cmd_Status( usbbase, msddisk );

//		IExec->DebugPrintF( "MSD_Bulk_Cmd_Status : Stat %ld\n", stat );

		if (( stat != PS_Okay ) && ( stat != PS_Stall ))
		{
			IExec->DebugPrintF( "-- Doing another try : Try cnt %ld\n", tries );

			MSD_DoBulk_Recovery( usbbase, msddisk );
			continue;
		}

		// --
		// Validate Status

		if ( LE_SWAP32( msddisk->msddisk_CSW.csw_Signature ) != MSDCSW_Signature )
		{
			IExec->DebugPrintF( "Status: Signature error = %08lx\n", msddisk->msddisk_CSW.csw_Signature );

			MSD_DoBulk_Recovery( usbbase, msddisk );
			continue;
		}

		host_Tag	= msddev->msddev_Bulk_TagID;
		csw_Tag		= LE_SWAP32( msddisk->msddisk_CSW.csw_Tag );

		if ( csw_Tag != host_Tag )
		{
			IExec->DebugPrintF( "Status: Tag ID Error = %08lx (expected %08lx)\n", csw_Tag, host_Tag );

			MSD_DoBulk_Recovery( usbbase, msddisk );
			continue;
		}

		residue	= LE_SWAP32( msddisk->msddisk_CSW.csw_DataResidue );
		actual	= (( dbs->dbs_Data ) && ( dbs->dbs_Data_Length )) ? (( dbs->dbs_Data_Length > residue ) ? ( dbs->dbs_Data_Length - residue ) : 0 ) : 0 ;

		if ( dbr )
		{
			dbr->csw_DataResidue= residue;
			dbr->csw_DataActual	= actual;
			dbr->csw_Valid		= TRUE;
			dbr->csw_Status		= msddisk->msddisk_CSW.csw_Status;
			dbr->csw_SigOK		= TRUE;
			dbr->csw_TagOK		= TRUE;
		}

		// Residue sanity check
		if ( residue > dbs->dbs_Data_Length )
		{
			IExec->DebugPrintF( "Status: residue %lu > CBW DataLen %lu\n", residue, dbs->dbs_Data_Length );

			MSD_DoBulk_Recovery( usbbase, msddisk );
			continue;
		}

		/**/ if ( msddisk->msddisk_CSW.csw_Status == MSDCSW_Status_CmdPassed )
		{
			// Optional strictness: enforce no-short unless SHORT_OK
			if (( dbs->dbs_Data ) && ( dbs->dbs_Data_Length ) && !( dbs->dbs_Flags & DOBULKF_SHORT_OK ) && ( residue ))
			{
				// Unexpected short; treat as transport error
				IExec->DebugPrintF( "Status: Unexpected short transfer, residue=%ld\n", residue );

				MSD_DoBulk_Recovery( usbbase, msddisk );
				continue;
			}

			retval = TRUE;
			break; // success
		}
		else if ( msddisk->msddisk_CSW.csw_Status == MSDCSW_Status_CmdFailed )
		{
			// Device reported CHECK CONDITION (from caller's POV)
			retval = FALSE;
			break; // let caller do (auto)sense
		}
		else // ( msddisk->msddisk_CSW.csw_Status == MSDCSW_Status_PhaseError )
		{
			IExec->DebugPrintF( "Status: Error %ld\n", msddisk->msddisk_CSW.csw_Status );

			MSD_DoBulk_Recovery( usbbase, msddisk );
			continue;
		}
	}

	TASK_NAME_LEAVE();
	return( retval );
}

// --
