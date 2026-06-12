
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Node_RemNode( struct USBBase *usbbase UNUSED, struct USB3_Header *header, PTR n )
{
struct USB3_Node *node = n;
struct USB3_Node *next;
struct USB3_Node *prev;

	next = node->un_Next;
	prev = node->un_Prev;

	if ( header->uh_Head == node )
	{
		// Remove Head

		if ( header->uh_Tail == node )
		{
			// There is only one node

			header->uh_Head = NULL;
			header->uh_Tail = NULL;
			header->uh_Nodes = 0;
		}
		else
		{
			// There are more than one node

			header->uh_Head = next;
			header->uh_Nodes--;

			next->un_Prev = NULL;
		}
	}
	else if ( header->uh_Tail == node )
	{
		// Remove Tail

		// There must be more than one node or else
		// the RemoveHead would have been triggered
		header->uh_Tail = prev;
		header->uh_Nodes--;

		prev->un_Next = NULL;
	}
	else
	{
		// Remove Middle

		next->un_Prev = prev;
		prev->un_Next = next;
		header->uh_Nodes--;
	}

	#if 0
//	#ifdef DO_PANIC
	node->un_Next = (PTR) 0xAC111111;
	node->un_Prev = (PTR) 0xAC222222;
	#else
	node->un_Next = NULL;
	node->un_Prev = NULL;
	#endif
}

// --
