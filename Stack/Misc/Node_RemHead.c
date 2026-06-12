
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE PTR __Node_RemHead( struct USBBase *usbbase UNUSED, struct USB2_Header *header )
{
struct USB2_Node *node;
struct USB2_Node *next;

	node = header->uh_Head;

	if ( ! node )
	{
		// No Nodes in Header

//		header->uh_Head = NULL;
		header->uh_Tail = NULL;
		header->uh_Nodes = 0;
	}
	else if ( header->uh_Tail == node )
	{
		// Head = Tail, so there is one Node

		#if 0
//		#ifdef DO_PANIC
		node->un_Next = (PTR) 0xAC111111;
		node->un_Prev = (PTR) 0xAC222222;
		#else
		node->un_Next = NULL;
		node->un_Prev = NULL;
		#endif

		header->uh_Head = NULL;
		header->uh_Tail = NULL;
		header->uh_Nodes = 0;
	}
	else
	{
		// More than one Node

		next = node->un_Next;

		next->un_Prev = NULL;

		#if 0
//		#ifdef DO_PANIC
		node->un_Next = (PTR) 0xAC111111;
		node->un_Prev = (PTR) 0xAC222222;
		#else
		node->un_Next = NULL;
		node->un_Prev = NULL;
		#endif

		header->uh_Head = next;
		header->uh_Nodes--;
	}

	return( node );
}

// --
