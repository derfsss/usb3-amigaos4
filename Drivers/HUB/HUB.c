
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) at
 * Carlstedt Research & Technology.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// --

#include "usb2_all.h"

#include "HUB.h"

// --
#if 0

#include "HUB_Wait.c"

#include "HUB__HUB_Feature_Clr.c"
#include "HUB__HUB_Status_Get.c"
#include "HUB__HUB_Change.c"

#include "HUB__Port_Status_Get.c"
#include "HUB__Port_Feature_Clr.c"
#include "HUB__Port_Feature_Set.c"
#include "HUB__Port_Function_Rem.c"
#include "HUB__Port_Function_Add3.c"
#include "HUB__Port_Function_Add2.c"
#include "HUB__Port_Function_Add.c"
#include "HUB__Port_Change.c"

#include "HUB_Get_Desc.c"
#include "HUB_Power_Up.c"
#include "HUB_Schedule.c"
#include "HUB_Interrupt.c"
#include "HUB_Stack.c"

#include "HUB_Main_1_Normal.c"
#include "HUB_Main_2_Signal.c"
#include "HUB_Main_3_Stopping.c"
#include "HUB_Main_4_Shutdown.c"
#include "HUB_Main.c"

#include "HUB_Init.c"
#include "HUB_Free.c"

#endif
// --

SEC_CODE U32 HUB_Entry( struct USBBase *usbbase, struct USB2_DriverMessage *msg )
{
struct intern *in;
U32 retval;

	TASK_NAME_ENTER( "HUB : HUB_Entry" );

	in = NULL;

	retval = TASK_Return_Error;

	// --

	struct RealFunctionNode *fn = (PTR) msg->Function;

	usbbase->usb_IExec->DebugPrintF( "USB3: HUB_Entry: addr=%ld tier=%ld port=%ld HCD=#%ld\n",
		(U32) fn->fkt_Address, (U32) fn->fkt_Tier, (U32) fn->fkt_PortNr,
		fn->fkt_HCD ? fn->fkt_HCD->hn_HCDIndex : -1 );

	#ifdef DO_PANIC

	if ( FUNCTION_VALID(fn) != VSTAT_Okay )
	{
		USBPANIC( "HUB : FN    %p : Invalid Pointer", fn );
	}

	#endif

	#ifdef DO_DEBUG
	U32 startadr = fn->fkt_Address;
	usbbase->usb_IExec->DebugPrintF( "\n##\n## HUB_Entry : Adr #%lu : Enter\n##\n\n", fn->fkt_Address );
	#endif

	// --

	#ifdef DO_INFO
	struct ExecIFace *IExec = usbbase->usb_IExec;
	IExec->Disable();
	IExec->DebugPrintF( "\nStartup new HUB\n" );
	IExec->DebugPrintF( "HCD Index   : %ld\n", fn->fkt_HCD->hn_HCDIndex );
	IExec->DebugPrintF( "Tier        : %ld\n", fn->fkt_Tier );
	IExec->DebugPrintF( "Addr Nr     : %ld\n", fn->fkt_Address );
	IExec->DebugPrintF( "Port Nr     : %ld\n\n", fn->fkt_PortNr );
	IExec->Enable();
	#endif

	if ( USB_HUB_MAX_DEPTH <= fn->fkt_Tier )
	{
		USBINFO( "HUB_Entry                : HUB Max Depth (%ld<=%ld)", USB_HUB_MAX_DEPTH, fn->fkt_Tier );
		goto bailout;
	}

	// --

	in = MEM_ALLOCVEC( sizeof( struct intern ), TRUE );

	if ( ! in )
	{
		USBDEBUG( "HUB_Entry                : Error allocating memory" );
		goto bailout;
	}

	#ifdef DO_DEBUG
	in->StructID = ID_IN_HUB;
	#endif

	in->StartMsg = msg;
	in->Function = (PTR) msg->Function;
	in->HCDNode = in->Function->fkt_HCD;
	in->HUB_ID = ( (U32) TASK_FIND() + 1 );

	USBDEBUG( "HUB_Entry                : HUB_ID $%08lx", in->HUB_ID );

	// --

	if ( ! HUB_Init( usbbase, in ))
	{
		USBDEBUG( "HUB_Entry                : Error Inilization Driver" );
		goto bailout;
	}

	// -- Peek into intern Stack Structure
	// we do this to make sure RootHUB driver
	// is started correct, so stack work.

	struct RealDriverMessage *rmsg = (PTR) msg;
	struct Task *parent;

	parent = rmsg->rdm_Parent;

	if ( parent )
	{
		rmsg->rdm_Parent = NULL;
		rmsg->rdm_Result = TASK_Return_Okay;
		TASK_SIGNAL( parent, SIGBREAKF_CTRL_E );
	}

	// --

	retval = TASK_Return_Okay;

	// Enable Ports
	HUB_Power_Up( usbbase, in );

	// Start Reading Interrupts
	HUB_Schedule( usbbase, in );

	// Handle Messages
	HUB_Main( usbbase, in );

	// --

bailout:

	if ( in )
	{
		HUB_Free( usbbase, in );
	
		MEM_FREEVEC( in );
		in = NULL;
	}

	// --

	#ifdef DO_DEBUG
	usbbase->usb_IExec->DebugPrintF( "\n##\n## HUB_Entry : Adr #%lu (%lu) : FN    %p : Locks %2ld : Leave\n##\n\n", fn->fkt_Address, startadr, fn, fn->fkt_Locks );
	#endif

	TASK_NAME_LEAVE();
	return( retval );
}

// --
