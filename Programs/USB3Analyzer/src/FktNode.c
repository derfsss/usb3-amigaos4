
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

struct USB3_FktNode *FktNode_Alloc( U32 type, USB3_ID id )
{
struct USB3_FktNode *n;

	n = NULL;

	if ( id < 200 )
	{
		DebugPrintF( "FktNode_Alloc : Invalid FktNode ID : %lld\n", id );
		goto bailout;
	}

	if ( type >= TYPE_LAST )
	{
		DebugPrintF( "FktNode_Alloc : Invalid Type ID : %ld\n", type );
		goto bailout;
	}

	n = AllocVecTags( sizeof( struct USB3_FktNode ),
		AVT_Clear, 0,
		TAG_END
	);

	if ( ! n )
	{
		goto bailout;
	}

	NewList( & n->Header );

	n->ID		= id;
	n->Type		= type;

bailout:

	return( n );
}

// --

void FktNode_Delete( struct USB3_FktNode *node )
{
PTR n;

	if ( ! node )
	{
		goto bailout;
	}

	while( TRUE )
	{
		n = RemHead( & node->Header );

		if ( ! n )
		{
			break;
		}

		FktNode_Delete( n );
	}

	if ( node->CfgDesc )
	{
		FreeVec( node->CfgDesc );
		node->CfgDesc = NULL;
	}

	if ( node->Functions )
	{
		FreeVec( node->Functions );
		node->Functions = NULL;
	}

	if ( node->Name )
	{
		FreeVec( (PTR)node->Name );
		node->Name = NULL;
	}

	FreeVec( node );

bailout:

	return;
}

// --

struct USB3_FktNode *FktNode_Find( struct List *l, USB3_ID id )
{
struct USB3_FktNode *node;
struct USB3_FktNode *tmp;

	node = NULL;

	if ( id < 200 )
	{
		DebugPrintF( "FktNode_Find : Invalid FktNode ID : %lld\n", id );
		goto bailout;
	}

	node = (PTR) GetHead( l );

	while( node )
	{
		if ( node->ID == id )
		{
			break;
		}
		else
		{
			if ( ! IsListEmpty( & node->Header ) )
			{
				tmp = FktNode_Find( & node->Header, id );

				if ( tmp )
				{
					node = tmp;		// Found
					break;
				}
			}

			node = (PTR) GetSucc( (PTR) node );
		}
	}

bailout:

	return(	node );
}

// --

void FktNode_Reset( struct USB3_FktNode *node )
{
PTR n;

	if ( ! node )
	{
		goto bailout;
	}

	node->ID = 0;

	while(( n = RemHead( & node->Header )))
	{
		FktNode_Delete( n );
	}

	if ( node->Type == TYPE_FUNCTION )
	{
		if ( node->CfgDesc )
		{
			FreeVec( node->CfgDesc );
			node->CfgDesc = NULL;
		}

		if ( node->Functions )
		{
			FreeVec( node->Functions );
			node->Functions = NULL;
		}
	}

	if ( node->Name )
	{
		node->Name[0] = '1';
	}

bailout:

	return;
}

// --
