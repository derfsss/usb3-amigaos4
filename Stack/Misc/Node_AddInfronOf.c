
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE void __Node_AddInfrontOf( struct USBBase *usbbase UNUSED, struct USB3_Header *header, PTR cur, PTR new )
{
struct USB3_Node *n = new;
struct USB3_Node *c = cur;
struct USB3_Node *t;

	#ifdef DO_DEBUG

	if ( ! header )
	{
		USBERROR( "Header NULL Pointer" );
		goto bailout;
	}

	if ( ! c )
	{
		USBERROR( "Cur NULL Pointer" );
		goto bailout;
	}

	if ( ! n )
	{
		USBERROR( "New NULL Pointer" );
		goto bailout;
	}

	#endif

	if (( ! c ) || ( ! c->un_Prev ))
	{
		NODE_ADDHEAD( header, n );
	}
	else
	{
		t = c->un_Prev;

		t->un_Next = n;

		n->un_Prev = t;
		n->un_Next = c;

		c->un_Prev = n;
	}

	#ifdef DO_DEBUG
	bailout:
	#endif
}
