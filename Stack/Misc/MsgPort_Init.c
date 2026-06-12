
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE S32 __MsgPort_Init( struct USBBase *usbbase, struct USB2_MsgPort *mp, STR file UNUSED )

#else

SEC_CODE S32 __MsgPort_Init( struct USBBase *usbbase, struct USB2_MsgPort *mp )

#endif

{
S32 retval;

	TASK_NAME_ENTER( "__MsgPort_Init" );

	retval = FALSE;

	#ifdef DO_PANIC

	if (( ! usbbase ) || ( ! mp ))
	{
		USBPANIC( "MsgPort_Init : 1 : NULL Pointer : (%s)", file );
	}

	if ( mp->ump_StructID == ID_USB2_MP )
	{
		USBPANIC( "MsgPort_Init : 2 : IN    use : (%s)", file );
	}

	#endif

	if ( ! TASK_ALLOCSIGNAL( & mp->ump_Signal ))
	{
		USBERROR( "MsgPort_Init : 3 : TASK_ALLOCSIGNAL failed : (%s)", file );
		goto bailout;
	}

	mp->ump_StructID						= ID_USB2_MP;
	mp->ump_MsgPort.mp_Node.ln_Type			= NT_MSGPORT;
	mp->ump_MsgPort.mp_MsgList.lh_Head		= (PTR) & mp->ump_MsgPort.mp_MsgList.lh_Tail;
	mp->ump_MsgPort.mp_MsgList.lh_Tail		= (PTR) NULL;
	mp->ump_MsgPort.mp_MsgList.lh_TailPred	= (PTR) & mp->ump_MsgPort.mp_MsgList.lh_Head;
	mp->ump_MsgPort.mp_SigTask				= TASK_FIND();
	mp->ump_MsgPort.mp_Flags				= PA_SIGNAL;
	mp->ump_MsgPort.mp_SigBit				= mp->ump_Signal.sig_Signal_Nr;

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
