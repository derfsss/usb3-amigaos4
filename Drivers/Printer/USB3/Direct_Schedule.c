
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

void Direct_Schedule( struct PrinterUnit *unit )
{
struct USB3_IORequest *usbio;
struct IOExtPar *pario;
U32 ionr;
U8 data[1];

	while( TRUE )
	{
		USBERROR( "Printer : Direct_Schedule : USB Active %ld : PAR Waiting %ld : BUF Waiting %ld : BUF Free %ld :",
			unit->unit_USB_IOReq_ActiveCount,
			unit->unit_PAR_IOReq_WaitingCount,
			unit->unit_Buffer_WaitingCount,
			unit->unit_Buffer_FreeCount
		);

		usbio = (PTR) GetHead( unit->unit_USB_IOReq_FreeList );

		USBERROR( "Printer : usbio %p", usbio );

		if ( ! usbio )
		{
			// No free USB IORequests
			USBINFO( "Direct_Schedule : No free USB IORequests" );
			break;
		}

		pario = (PTR) GetHead( unit->unit_PAR_IOReq_WaitingList );

		USBERROR( "Printer : pario %p", pario );

		if ( ! pario )
		{
			// Okay nothing too print
			USBINFO( "Direct_Schedule : Okay nothing too print" );

			if (( unit->unit_Shutdown ) 
			&&	( ! unit->unit_USB_IOReq_ActiveCount )
			&&	( ! unit->unit_PAR_IOReq_WaitingCount )
			&&	( ! unit->unit_Buffer_WaitingCount ))
			{
				USBERROR( "Last byte : $%02lx (%lu)", (U32) unit->unit_LastByteSend, (U32) unit->unit_LastByteSend );
				if (( unit->unit_LastByteSend != '\f' ) && ( ! unit->unit_Detached ))
				{
					data[0] = '\f';		// FormFeed
					usbio = unit->unit_Bulk_Tx_Resource->IORequests[0];
					usbio->io_Command	= CMD_WRITE;
					usbio->io_UserData	= NULL;
				//	usbio->io_Offset	= 0;
					usbio->io_Length	= 1;
					usbio->io_Data		= data;
					DoIO( (PTR) usbio );
				}

				// --

				USBINFO( "Direct_Schedule : We are done, time to exit" );
				unit->unit_Running = FALSE;
			}
			else
			{
				USBINFO( "Direct_Schedule : Okay nothing too print" );
			}

			break;
		}

		// Get IOReq nr
		ionr = usbio->io_UserValue;

		#ifdef DO_DEBUG

		if ( unit->unit_Bulk_Tx_Active[ionr] )
		{
			USBERROR( "unit_Bulk_Tx_Active[%ld] is not Free", ionr );
			break;
		}

		#endif

		// Move from Waiting/Free list into Active
		Remove( (PTR) pario );
		Remove( (PTR) usbio );

		unit->unit_Bulk_Tx_Active[ionr] = usbio;
		unit->unit_USB_IOReq_ActiveCount++;
		unit->unit_PAR_IOReq_WaitingCount--;

		USBERROR( "Sending : PAR : Data %p : Len %lu", pario->IOPar.io_Data, pario->IOPar.io_Length );

		// Init and Start USB IORequest
		usbio->io_Command	= CMD_WRITE;
		usbio->io_UserData	= pario;
//		usbio->io_Offset	= 0;
		usbio->io_Length	= pario->IOPar.io_Length;
		usbio->io_Data		= pario->IOPar.io_Data;

		SendIO( (PTR) usbio );
	}


}

// --
