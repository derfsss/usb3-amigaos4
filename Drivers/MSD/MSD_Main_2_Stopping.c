
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --
#if 1

// All LUNs free of openers? The filesystems close their units after
// the Mounter dismounts the (already denounced) volumes.

SEC_CODE static S32 __AllUnitsClosed( struct MSDDevice *msddev )
{
struct MSDDisk *msddisk;
PTR node;

	node = msddev->msddev_MSDDisk_Header.uh_Head;

	while( node )
	{
		msddisk = (PTR) node;

		if ( msddisk->msddisk_OpenCnt > 0 )
		{
			return( FALSE );
		}

		node = Node_Next( node );
	}

	return( TRUE );
}

// --

SEC_CODE void MSD_Main_2_Stopping( struct USBBase *usbbase, struct MSDDevice *msddev )
{
U32 wait;
U32 mask;

	TASK_NAME_ENTER( "MSD : MSD_Main_2_Stopping" );
	USBERROR( "MSD : MSD_Main_2_Stopping" );

	msddev->Running = TRUE;

	// -- Normal mode

	wait  = SIGBREAKF_CTRL_C;
	wait |=	msddev->msddev_Register->reg_Public.Stack_MsgPortBit;
	wait |= msddev->msddev_MsgPort_Abort.ump_Signal.sig_Signal_Mask;
	wait |= msddev->msddev_MsgPort_Begin.ump_Signal.sig_Signal_Mask;

	while( msddev->Running )
	{
		// Done when the last opener is gone -- _man_Close signals the
		// Begin port so this is re-evaluated promptly
		if ( __AllUnitsClosed( msddev ))
		{
			USBERROR( "MSD : MSD_Main_2_Stopping : all units closed, exiting" );
			msddev->Running = FALSE;
			break;
		}

		mask = TASK_WAIT( wait );

		if ( mask & msddev->msddev_MsgPort_Abort.ump_Signal.sig_Signal_Mask )
		{
			USBERROR( "MSD : __myHandle_Abort" );

			MSD_Main__myHandle_Abort( usbbase, msddev );
		}

		if ( mask & msddev->msddev_MsgPort_Begin.ump_Signal.sig_Signal_Mask )
		{
//			USBERROR( "MSD : __myHandle_Begin" );

			MSD_Main__myHandle_Begin( usbbase, msddev );
		}

		if ( mask & msddev->msddev_Register->reg_Public.Stack_MsgPortBit )
		{
			USBDEBUG( "MSD : Got Stack Msg" );

			MSD_Main__myHandle_Stack( usbbase, msddev );
		}

		// SIGBREAKF_CTRL_C needs no handler -- the loop re-checks the
		// exit condition at the top on every wakeup
	}

	TASK_NAME_LEAVE();
}

#endif
// --
