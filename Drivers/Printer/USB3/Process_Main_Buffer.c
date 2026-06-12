
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

static void __myProcess_Main_Buffer( struct PrinterUnit *unit )
{
struct USB3_IORequest *usbio;
struct PrtBuffer *pb;
U32 ionr;

	while( TRUE )
	{
		USBERROR( "Printer : Main_Buffer : USB Active %ld : PAR Waiting %ld : BUF Waiting %ld : BUF Free %ld :",
			unit->unit_USB_IOReq_ActiveCount,
			unit->unit_PAR_IOReq_WaitingCount,
			unit->unit_Buffer_WaitingCount,
			unit->unit_Buffer_FreeCount
		);

		usbio = (PTR) GetMsg( unit->unit_Bulk_Tx_Resource->MsgPort );

		USBERROR( "__myProcess_Main_Buffer : ioreq %p", usbio );

		if ( ! usbio )
		{
			break;
		}

		ionr = usbio->io_UserValue;
		pb = usbio->io_UserData;

		USBERROR( "__myProcess_Main_Buffer : pb %p (#%lu)", pb, ionr );

		// --

		#ifdef DO_DEBUG

		if ( ! pb )
		{
			USBERROR( "pb NULL Pointer" );
		}

		if ( ! unit->unit_Bulk_Tx_Active[ionr] )
		{
			USBERROR( "unit_Bulk_Tx_Active[%ld] is not Active", ionr );
		}

		if ( unit->unit_Bulk_Tx_Active[ionr] != usbio )
		{
			USBERROR( "unit_Bulk_Tx_Active[%ld] is wrong pointer", ionr );
		}

		#endif

		unit->unit_Bulk_Tx_Active[ionr] = NULL;
		unit->unit_USB_IOReq_ActiveCount--;

		// --

		switch( usbio->io_Error )
		{
			case USB3Err_NoError:
			{
				USBERROR( "__myProcess_Main_Buffer : USB3Err_NoError" );

				if (( usbio->io_Data ) &&  ( usbio->io_Actual > 0 ))
				{
					U8 *buf = usbio->io_Data;
					U32 off = usbio->io_Actual-1;
					unit->unit_LastByteSend = buf[ off ];
				}
				break;
			}

			case USB3Err_Device_Detached:
			{
				// Todo : Flush all queued requests
				USBERROR( "__myProcess_Main_Buffer : USB3Err_Device_Detached" );
				unit->unit_Detached = TRUE;
				break;
			}

			default:
			{
				USBERROR( "__myProcess_Main_Buffer : Unknown error %lu", (U32) usbio->io_Error );
				break;
			}
		}

		// --

//		USBERROR( "usbio->io_Offset  : %ld", usbio->io_Offset );
//		USBERROR( "usbio->io_Length  : %ld", usbio->io_Length );
//		USBERROR( "usbio->io_Actual  : %ld", usbio->io_Actual );
//		USBERROR( "usbio->io_Error   : %ld", usbio->io_Error );

		USBERROR( "__myProcess_Main_Buffer : Queueing USB IOReq %p", usbio );
		AddTail( unit->unit_USB_IOReq_FreeList, (PTR) usbio );

		if ( unit->unit_Buffer_FreeCount < 3 )
		{
			USBERROR( "__myProcess_Main_Buffer : Queueing PB %p", pb );
			AddTail( unit->unit_Buffer_FreeList, & pb->pb_Node );
			unit->unit_Buffer_FreeCount++;
		}
		else
		{
			USBERROR( "__myProcess_Main_Buffer : Freeing PB %p", pb );
			FreeVec( pb );
		}
	}

	if ( ! unit->unit_Detached )
	{
		Buffer_Schedule( unit );
	}
}
