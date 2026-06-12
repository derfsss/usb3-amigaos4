
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_ALL_H
#error Include "inc/All.h" first
#endif

#ifndef INC_PROTOS_H
#define INC_PROTOS_H

// --
// FktNode.c
struct USB3_FktNode *	FktNode_Alloc( U32 type, USB3_ID id );
void					FktNode_Delete( struct USB3_FktNode *n );
struct USB3_FktNode *	FktNode_Find( struct List *l, USB3_ID id );
void					FktNode_Reset( struct USB3_FktNode *n );

// --
// MainScreen.c
S32		Main_OpenScreen( void );
void	Main_CloseScreen( void );

// --
// MainWindow.c
S32		Main_OpenWindow( void );
void	Main_CloseWindow( void );
S32		Main_BuildWindow( void );
void	Main_DestoryWindow( void );
void	Main_HandleWindow( void );
void	Build_Device_Tree( void );

// --
// Notify.c
void	Handle_Notify( void );

// --
// Resources.c
S32		myOpenResources( int );
void	myCloseResources( void );

// --
// USB.c
S32		USB_Setup( void );
void	USB_Cleanup( void );
STR		GetClassName( U32 class, U32 subclass, U32 protocol );


// --

#endif