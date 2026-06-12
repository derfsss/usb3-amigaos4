
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --
// Real cmd name : PDCMD_QUERY

static void Cmd_PDQuery( struct PrinterUnit *unit, struct IOExtPar *pario )
{
struct USB3_EPResource *epr;
struct USB3_IORequest *usbio;
struct USB3_SetupData *sd;

	// --

	/*
	** We are a Parallel Type device
	**
	** Serial Status is 2 bytes
	** Parallel Status is only 1 byte
	** 
	*/

	epr = unit->unit_USB_Register->Res_Control;
	sd = epr->SetupData;

	sd->RequestType		= REQTYPE_Read | REQTYPE_Class | REQTYPE_Interface;
	sd->RequestCode		= USBPRT_REQC_GET_PORT_STATUS;
	sd->Value			= LE_SWAP16( 0 );
	sd->Index			= LE_SWAP16( unit->unit_USB_Interface->IFC_Num );
	sd->Length			= LE_SWAP16( 1 );

	// --

	usbio = epr->IORequests[0];

	usbio->io_Command	= CMD_READ;
//	usbio->io_Offset	= 0;
	usbio->io_Length	= 1;	// USB Printer status is 1 byte
	usbio->io_Data		= & unit->unit_PrinterStatus;

	DoIO( (PTR) usbio );

	// --

	switch( usbio->io_Error )
	{
		case USB3Err_NoError:
		{
			USBERROR( "Cmd_PDQuery : USB3Err_NoError" );
			pario->IOPar.io_Actual = 1;		// Parallel stat size
			pario->IOPar.io_Error = 0;
			pario->io_Status = 0;

			if ( unit->unit_PrinterStatus & USBPRT_STAT_EMPTY )
			{
				pario->io_Status |= IOPTF_PAPEROUT;
			}
			else
			{
				pario->io_Status &= ~IOPTF_PAPEROUT;
			}

			if ( unit->unit_PrinterStatus & USBPRT_STAT_SELECTED )
			{
				pario->io_Status |= IOPTF_PARBUSY;
			}
			else
			{
				pario->io_Status &= ~IOPTF_PARBUSY;
			}
			break;
		}

		case USB3Err_Device_Detached:
		{
			USBERROR( "Cmd_PDQuery : USB3Err_Device_Detached" );
			pario->IOPar.io_Actual = 0;
			pario->IOPar.io_Error = ParErr_NotOpen;
			pario->io_Status = 0;
			unit->unit_Detached = TRUE;
			break;
		}

		default:
		{
			USBERROR( "Cmd_PDQuery : Unknown error %lu", (U32) usbio->io_Error );
			pario->IOPar.io_Actual = 0;
			pario->IOPar.io_Error = ParErr_LineErr;
			pario->io_Status = 0;
			break;
		}
	}

	// --

	USBERROR( "Cmd_PDQuery : Reply IOReq %p", pario );

	ReplyMsg( (PTR) pario );
}

// --
