
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

SEC_CODE static void myGetHead( struct USBBase *usbbase, struct RealFunctionNode **ptr_fn, struct USB3_Node **ptr_n )
{
struct RealFunctionNode *fn;
struct USB3_Node *n;

	SEMAPHORE_OBTAIN( & usbbase->usb_Fkt_Semaphore );

	n = Header_Head( & usbbase->usb_Fkt_Header );

	if ( n )
	{
		fn = n->un_Data;

		if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
		{
			fn = NULL;
			n  = NULL;
		}
	}
	else
	{
		fn = NULL;
		n  = NULL;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Fkt_Semaphore );

	*ptr_fn = fn;
	*ptr_n = n;
}

// --

SEC_CODE static void myGetNext( struct USBBase *usbbase, struct RealFunctionNode **ptr_fn, struct USB3_Node **ptr_n )
{
struct RealFunctionNode *fn;
struct USB3_Node *n;

	n  = *ptr_n;
	fn = *ptr_fn;

	SEMAPHORE_OBTAIN( & usbbase->usb_Fkt_Semaphore );

	FUNCTION_UNLOCK( fn );

	// --

	n = n->un_Next;

	if ( n )
	{
		fn = n->un_Data;

		if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
		{
			fn = NULL;
			n  = NULL;
		}
	}
	else
	{
		fn = NULL;
		n  = NULL;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Fkt_Semaphore );

	*ptr_fn = fn;
	*ptr_n = n;
}

// --
// Trying to avoid holding the semaphore lock for a long time
// so thats why I use GetHead/GetNext .. I know it looks ugly

static void __myProcess_Entry( void )
{
struct RealFunctionNode *fn;
struct MasterStartMsg *tm;
struct ExecIFace *IExec;
struct USB3_Node *n;
struct USBBase *usbbase;

	// --

	TASK_NAME_ENTER( "__myProcess_LaunchDrivers" );

	IExec	= (PTR)(*(struct ExecBase **)4)->MainInterface;
	tm		= (PTR)(IExec->FindTask(NULL)->tc_UserData);
	usbbase	= tm->usbbase;

	TASK_SIGNAL( tm->Parent, tm->Mask );

	myGetHead( usbbase, & fn, & n );

	while( n )
	{
		// This will not work on RootHub Device/Function
		// but we will never get to add a driver as
		// our HUB driver have takes control of that device

		FUNCTION_BIND( fn, fn->fkt_ASync );

		myGetNext( usbbase, & fn, & n );
	}

	TASK_NAME_LEAVE();
}

// --

SEC_CODE static void __myProcess_LaunchDrivers( struct USBBase *usbbase, struct intern *in UNUSED )
{
struct MasterStartMsg msg;
struct USB3_Signal sig;
struct DOSIFace *IDOS;
U32 mask;

	if ( TASK_ALLOCSIGNAL( & sig ))
	{
		IDOS = usbbase->usb_IDOS;

 		msg.usbbase = usbbase;
		msg.Parent = TASK_FIND();
		msg.Result = FALSE;
		msg.Mask = sig.sig_Signal_Mask;

		if ( IDOS->CreateNewProcTags( 
			NP_Name,		"Universal Serial Bus - Launcher",
			NP_Child,		FALSE,
			NP_Priority,	1,
			NP_StackSize,	1024 * 32,
			NP_Entry,		__myProcess_Entry,
			NP_UserData,	& msg,
			TAG_END ))
		{
			while( TRUE )
			{
				mask = TASK_WAIT( msg.Mask );

				if (( mask & msg.Mask ) == msg.Mask )
				{
					break;
				}
			}
		}
		else
		{
			USBERROR( "__myProcess_LaunchDrivers : Process error" );
		}

		TASK_FREESIGNAL( & sig );
	}
	else
	{
		USBERROR( "__myProcess_LaunchDrivers : Signal error" );
	}
}

// --
