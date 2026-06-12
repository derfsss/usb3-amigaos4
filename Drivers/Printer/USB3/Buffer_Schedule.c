
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

void Buffer_Schedule( struct PrinterUnit *unit )
{
struct USB3_IORequest *usbio;
struct PrtBuffer *pb;
U32 ionr;

	while( TRUE )
	{
		USBERROR( "Printer : Buffer_Schedule : USB Active %ld : PAR Waiting %ld : BUF Waiting %ld : BUF Free %ld :",
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
			USBINFO( "Buffer_Schedule : No free USB IORequests" );
			break;
		}

		pb = (PTR) GetHead( unit->unit_Buffer_WaitingList );

		USBERROR( "Printer : pb %p", pb );

		if ( ! pb )
		{
			// Okay nothing too print

			if (( unit->unit_Shutdown ) 
			&&	( ! unit->unit_USB_IOReq_ActiveCount )
			&&	( ! unit->unit_PAR_IOReq_WaitingCount )
			&&	( ! unit->unit_Buffer_WaitingCount ))
			{
				USBINFO( "Buffer_Schedule : We are done, time to exit" );
				unit->unit_Running = FALSE;
			}
			else
			{
				USBINFO( "Buffer_Schedule : Okay nothing too print" );
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
		Remove( (PTR) pb );
		Remove( (PTR) usbio );

		unit->unit_Bulk_Tx_Active[ionr] = usbio;
		unit->unit_USB_IOReq_ActiveCount++;
		unit->unit_Buffer_WaitingCount--;

		USBERROR( "Sending : PB : Data %p : Pos %lu : Free %lu", pb->pb_Data, pb->pb_Pos, pb->pb_Free );

		// Init and Start USB IORequest
		usbio->io_Command	= CMD_WRITE;
		usbio->io_UserData	= pb;
//		usbio->io_Offset	= 0;
		usbio->io_Length	= pb->pb_Pos;
		usbio->io_Data		= pb->pb_Data;

		SendIO( (PTR) usbio );
	}
}

// --
