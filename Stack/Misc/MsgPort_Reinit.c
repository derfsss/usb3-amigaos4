
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

/*
** Running under Disable() and must not fail.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void __MsgPort_Reinit( struct USBBase *usbbase, struct USB3_MsgPort *mp, STR file UNUSED )

#else

SEC_CODE void __MsgPort_Reinit( struct USBBase *usbbase, struct USB3_MsgPort *mp )

#endif

{
	TASK_NAME_ENTER( "__MsgPort_Reinit" );

	#ifdef DO_PANIC

	if ( ! mp )
	{
		USBPANIC( "__MsgPort_Reinit : 1 : NULL Pointer : %s", file );
	}

	if (( mp->ump_StructID ) && ( mp->ump_StructID != ID_USB3_MP ))
	{
		USBPANIC( "__MsgPort_Reinit : 2 : Invalid : %s", file );
	}

	#endif

	// Lets just overwrite old Signals
	mp->ump_Signal.sig_StructID = 0;

	if ( ! TASK_ALLOCSIGNAL( & mp->ump_Signal ))
	{
		USBERROR( "TASK_ALLOCSIGNAL failed\n" );
	}

	// Don't call MSGPORT_INIT as we need
	// too keep the List and its nodes

	mp->ump_MsgPort.mp_SigTask = TASK_FIND();
	mp->ump_MsgPort.mp_SigBit = mp->ump_Signal.sig_Signal_Nr;

	TASK_NAME_LEAVE();
}

// --
