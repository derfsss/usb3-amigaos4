
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

SEC_CODE void MSD_Main_1_Normal( struct USBBase *usbbase, struct MSDDevice *msddev )
{
U32 wait;
U32 mask;

	TASK_NAME_ENTER( "MSD : MSD_Main_1_Normal" );
	USBERROR( "MSD : MSD_Main_1_Normal" );

	msddev->Running = TRUE;

	// -- Normal mode

	wait  = SIGBREAKF_CTRL_C;
	wait |=	msddev->msddev_Register->reg_Public.Stack_MsgPortBit;
	wait |= msddev->msddev_MsgPort_Abort.ump_Signal.sig_Signal_Mask;
	wait |= msddev->msddev_MsgPort_Begin.ump_Signal.sig_Signal_Mask;

	while( msddev->Running )
	{
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

		if ( mask & SIGBREAKF_CTRL_C )
		{
			USBDEBUG( "MSD : Got CTRL+C" );

			msddev->Running = FALSE;
		}
	}

	TASK_NAME_LEAVE();
}

#endif
// --
