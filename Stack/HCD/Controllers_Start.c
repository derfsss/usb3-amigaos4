
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

#include "HCD.h"

// --


// --

#include "HCD_1_Init.c"
#include "HCD_2_Main.c"
#include "HCD_3_Free.c"

// --

SEC_RWDATA static STR HCDNames[HCDTYPE_Last] =
{
	[HCDTYPE_UHCI]		= "UHCI (PCI)",
	[HCDTYPE_OHCI]		= "OHCI (PCI)",
	[HCDTYPE_EHCI]		= "EHCI (PCI)",
	[HCDTYPE_XHCI]		= "XHCI (PCI)",
	[HCDTYPE_SAM440]	= "OHCI (Onboard)",
	[HCDTYPE_SAM460]	= "OHCI (Onboard)",
	[HCDTYPE_P5020]		= "EHCI (Onboard)",
	[HCDTYPE_P5040]		= "EHCI (Onboard)",
	[HCDTYPE_TABOR]		= "EHCI (Onboard)",
};

SEC_CODE S32 __HCD_Controllers_Start( struct USBBase *usbbase )
{
struct RealFunctionNode *fn;
struct USB2_TaskNode *tn;
struct USB2_HCDNode *hn;
enum TaskReturn stat;
struct Task *master;
S32 retval;
STR name;
S32 err;
S32 pri;

	TASK_NAME_ENTER( "HCD : __HCD_Controllers_Start" );

	retval = FALSE;

	// -- Figure out what pri to run HCD at

	master = usbbase->usb_Master_Task.tn_TaskAdr;

	if ( ! master )
	{
		USBERROR( "__HCD_Controllers_Start : NULL Pointer" );
		goto bailout;
	}

	pri = master->tc_Node.ln_Pri;
	pri -= 1;				// One under Master
	pri = MAX( pri, 1 ); 	// Need minimum pri 1
	pri = MIN( pri, 50 );	// and  maximum pri 50

	// --

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## __HCD_Controllers_Start   :  1 :\n##\n\n" );

	hn = Header_Head( & usbbase->usb_HCDHeader );

	while( hn )
	{
		err = TRUE;

		while( TRUE )
		{
			name = ( hn->hn_HCDType < HCDTYPE_Last ) ? HCDNames[hn->hn_HCDType] : " HCD" ;

			tn = NULL;

			// Start HCD process and setup Root HUB Device 
			// (not hub driver)
			stat = TASK_START(
				TASK_Tag_InternSize, sizeof( struct intern ),
				TASK_Tag_Func_Init, __myInit,
				TASK_Tag_Func_Main, __myMain,
				TASK_Tag_Func_Free, __myFree,
				TASK_Tag_UserData, hn,
				TASK_Tag_Prioity, pri,
				TASK_Tag_Get_TN, & tn,
				TASK_Tag_ASync, & usbbase->usb_HCDASync,
				TASK_Tag_Type, TASK_Type_HCD,
				TASK_Tag_Name, name,
				TAG_END	
			);

//			usbbase->usb_IExec->DebugPrintF( "\n##\n## __HCD_Controllers_Start   :  1 : Stat $%08lx\n##\n\n", (U32) stat );

			if ( myIS_TASKRETURN_ERR(stat) )
			{
				USBERROR( "__HCD_Controllers_Start   : 1.1 : Failed to start HCD Task (Stat $%08lx)", stat );
				break;
			}

			if ( TASK_VALID(tn) != VSTAT_Okay )
			{
				USBERROR( "__HCD_Controllers_Start   : 1.2 : Failed to start HCD Task (TN %p)", tn );
				break;
			}

			TASK_SETTLE(tn);

			// well we may fall throu Main and exit.. just checking its still running
			if (( ! tn->tn_TaskAdr ) || ( tn->tn_State != TASK_State_Running ))
			{
				USBERROR( "__HCD_Controllers_Start   :  2 : Error Task not Running" );
				break;
			}

			fn = hn->hn_Function_Array[0];

//			usbbase->usb_IExec->DebugPrintF( "Calling Function Init\n" );

			// Init Root HUB Device, by reading info via USB IO
			stat = FUNCTION_INIT( fn, & hn->hn_HUB_ASync );
			// Init return True/False

//			usbbase->usb_IExec->DebugPrintF( "Function Init : Stat %lx\n", stat );

//			usbbase->usb_IExec->DebugPrintF( "\n##\n## __HCD_Controllers_Start   : 2.1 : Stat %ld\n##\n\n", (S32) stat );

			if ( ! stat )
			{
				USBERROR( "__HCD_Controllers_Start   :  3 : Error Function Init Failed" );
				break;
			}

//			usbbase->usb_IExec->DebugPrintF( "Calling Function Bind\n" );

			// Try and Bind Function to a Driver
			stat = FUNCTION_BIND( fn, & hn->hn_HUB_ASync );
			// Bind return True/False

//			usbbase->usb_IExec->DebugPrintF( "Function Bind : Stat %lx\n", stat );

//			usbbase->usb_IExec->DebugPrintF( "\n##\n## __HCD_Controllers_Start   :  4 : Stat %ld\n##\n\n", (S32) stat );

			if ( ! stat )
			{
				USBERROR( "__HCD_Controllers_Start   : Error: HN    Start 3" );
				break;
			}

//			usbbase->usb_IExec->DebugPrintF( "\n##\n## __HCD_Controllers_Start   :  5 : yay\n##\n\n" );

//			usbbase->usb_IExec->DebugPrintF( "Function Done\n" );

			err = FALSE;
			break;
		}

		if ( err )
		{
			// do resource free if needed
//			usbbase->usb_IExec->DebugPrintF( "\n##\n## __HCD_Controllers_Start   :  6 : failed\n##\n\n" );
			USBERROR( "__HCD_Controllers_Start   : Error: Failed to start hcd" );
			goto bailout;
		}

		hn = Node_Next( hn );
	}

	retval = TRUE;

bailout:

//	usbbase->usb_IExec->DebugPrintF( "\n##\n## __HCD_Controllers_Start   :  2 : retval %ld\n##\n\n", retval );

	TASK_NAME_LEAVE();

	return( retval );
}

// --
