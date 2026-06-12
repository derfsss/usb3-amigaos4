
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Node_AddHead( struct USBBase *usbbase UNUSED, struct USB2_Header *header, PTR n )
{
struct USB2_Node *node = n;
struct USB2_Node *head;

	head = header->uh_Head;

	if ( ! head )
	{
		node->un_Next = NULL;
		node->un_Prev = NULL;

		header->uh_Head = node;
		header->uh_Tail = node;
		header->uh_Nodes = 1;
	}
	else
	{
		node->un_Next = head;
		node->un_Prev = NULL;

		head->un_Prev = node;

		header->uh_Head = node;
		header->uh_Nodes++;
	}
}

// --
