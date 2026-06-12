
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

void Master_Promote_Entry( void );

// --

SEC_CODE S32 __myMaster_StartPromote( struct USBBase *usbbase )
{
struct MasterStartMsg msg;
struct ExecIFace *IExec;
struct DOSIFace *IDOS;
S32 retval;
PTR task;
U32 mask;

	retval = FALSE;

	if ( usbbase->usb_Master_Task.tn_TaskAdr )
	{
		IExec = usbbase->usb_IExec;

		if ( ! usbbase->usb_DOSBase )
		{
			usbbase->usb_DOSBase = IExec->OpenLibrary( "dos.library", 50 );
		}

		if ( ! usbbase->usb_IDOS )
		{
			usbbase->usb_IDOS = (PTR) IExec->GetInterface( usbbase->usb_DOSBase, "main", 1, NULL );
		}

		IDOS = usbbase->usb_IDOS;

		if ( IDOS )
		{
			msg.usbbase = usbbase;
			msg.Parent = TASK_FIND();
			msg.Result = FALSE;
			msg.Mask = SIGBREAKF_CTRL_E;

			task = IDOS->CreateNewProcTags( 
				NP_Name,		"Universal Serial Bus",
				NP_Child,		FALSE,
				NP_Priority,	120,		// avoid task switches
				NP_StackSize,	1024 * 32,
				NP_Entry,		Master_Promote_Entry,
				NP_UserData,	& msg,
				TAG_END
			);

			while( task )
			{
				mask = IExec->Wait( msg.Mask );

				if (( mask & msg.Mask ) == msg.Mask )
				{
					retval = msg.Result;
					break;
				}
			}
		}
	}

	return( retval );
}

// --
