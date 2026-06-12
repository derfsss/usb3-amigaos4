
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void _main_Notify_Remove( struct USB3IFace *Self, struct USB3_NotifyNode *node )
{
struct USB3_NotifyNode *n;
struct USBBase *usbbase;

	usbbase = (PTR) Self->Data.LibBase;

	if ( ! node )
	{
		goto bailout;
	}

	if ( node->nn_StructID != ID_USB3_NN )
	{
		USBERROR( "Error this is not a Notify Node" );
		goto bailout;
	}

	SEMAPHORE_OBTAIN( & usbbase->usb_Notify_Semaphore );

	n = usbbase->usb_Notify_Header.uh_Head;

	while( n )
	{
		if ( n == node )
		{
			break;
		}
		else
		{
			n = n->nn_Node.un_Next;
		}
	}

	if ( n )
	{
		#ifdef DEBUG
		n->nn_StructID = 0;
		#endif

		NODE_REMNODE( & usbbase->usb_Notify_Header, n );

		MEMORY_FREE( MEMID_NotifyNode, n, 0 );
	}
	#ifdef DEBUG
	else
	{
		USBERROR( "Node not found" );
	}
	#endif

	SEMAPHORE_RELEASE(	& usbbase->usb_Notify_Semaphore );

bailout:

	return;
	
}

// --
