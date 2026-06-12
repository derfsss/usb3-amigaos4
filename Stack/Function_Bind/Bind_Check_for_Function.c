
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __myFKT_Entry( struct USBBase *usbbase, PTR userdata, PTR in UNUSED )
{
struct USB2_DriverNode *dn;
struct Task *parent;
U32 stat;

	TASK_NAME_ENTER( "__myFKT_Entry" );

	dn = userdata;

	USBDEBUG( "__myFKT_Entry            :  1 : DN %p : Call Driver", dn );
//	usbbase->usb_IExec->DebugPrintF( "__myFKT_Entry            :  1 :\n" );

	// --

	if ( DRIVER_LOCK( dn ) == LSTAT_Okay )
	{
		USBDEBUG( "__myFKT_Entry            :  3 : DN %p : Entry %p", dn->dn_Entry );

		if ( dn->dn_Entry )
		{
			stat = dn->dn_Entry( usbbase, & dn->dn_Message.rdm_Public );
		}
		else
		{
			USBERROR( "__myFKT_Entry : dn_Entry NULL Pointer" );
			stat = TASK_Return_Error;
		}

		// Driver Quit
		dn->dn_FreeMe = TRUE;
	
		// --

		parent = dn->dn_Message.rdm_Parent;
		
		if ( parent )
		{
			dn->dn_Message.rdm_Parent = NULL;
			dn->dn_Message.rdm_Result = stat;
			USBDEBUG( "HUB Sending CTRL+E to %p (Stat $%08lx)", parent, stat );
			TASK_SIGNAL( parent, SIGBREAKF_CTRL_E );
		}

		// --

		DRIVER_UNLOCK( dn );
	}

	// --

//	usbbase->usb_IExec->DebugPrintF( "__myFKT_Entry            :  2 :\n" );
	USBDEBUG( "__myFKT_Entry            :  2 : DN %p : Driver Exit", dn );

	TASK_NAME_LEAVE();
}

// --

SEC_CODE static U32 _Start_Function( 
	struct USBBase *usbbase, 
	struct RealFunctionNode *fn,
	struct USB2_FktDriverNode *fdn,
	struct USB2_ASync *as )
{
struct USB2_DriverNode *dn;
struct USB2_TaskNode *tn;
enum TaskReturn stat;
S32 isRootHUB;
U32 retval;

	TASK_NAME_ENTER( "_Start_Function" );

	retval = TASK_Return_Stack_Error;

	dn = DRIVER_ALLOC( fn, as );

	if ( ! dn )
	{
		USBERROR( "_Start_Function          : Error allocating memory" );
		goto bailout;
	}

	dn->dn_Entry = fdn->fdn_Entry;
	dn->dn_Filename = fdn->fdn_Driver_Filename;

	// -- Init Public Message

	// Alloc check FN
//	if ( FUNCTION_VALID(fn) != VSTAT_Okay )
//	{
//		USBDEBUG( "_Start_Function : Function Valid error (%p)", fn );
//		goto bailout;
//	}

	dn->dn_Message.rdm_Public.IUSB2 = usbbase->usb_IUSB2;
	dn->dn_Message.rdm_Public.Function = (PTR) fn;
	dn->dn_Message.rdm_Public.ConfigDescriptors = (PTR) fn->fkt_Config_Desc_Buf;

	// --

	if (( fn->fkt_Tier == 0 ) // RootHub
	&&	( fn->fkt_Class == USBCLASS_HUB ))
	{
		isRootHUB = TRUE;

		USBDEBUG( "_Start_Function : is RootHUB" );
		dn->dn_Message.rdm_Parent = TASK_FIND();
	}
	else
	{
		isRootHUB = FALSE;

		USBDEBUG( "_Start_Function : is NOT RootHUB" );
		dn->dn_Message.rdm_Parent = NULL;
	}

	// -- Start Driver

	stat = TASK_START(
		TASK_Tag_Func_Main, __myFKT_Entry,
		TASK_Tag_UserData, dn,
		TASK_Tag_Get_TN, & tn,
		TASK_Tag_Prioity, fdn->fdn_Pri,
		TASK_Tag_ASync, as,
		TASK_Tag_Type, TASK_Type_FKT,
		TASK_Tag_Name, fdn->fdn_Title,
		TAG_END	
	);

	// --

	if (( TASKRETURN_OK(stat) ) && ( isRootHUB ))
	{
		USBDEBUG( "_Start_Function          : Waiting for HUB Responce ( CTRL+E )" );

		TASK_WAIT( SIGBREAKF_CTRL_E );

		USBDEBUG( "_Start_Function          : Got HUB Responce ( Result : $%08lx )", dn->dn_Message.rdm_Result );

		stat = dn->dn_Message.rdm_Result;
	}

	// --

	retval = stat;

	if ( TASKRETURN_OK(stat) )
	{
		USBDEBUG( "_Start_Function          : Okay (Stat $%08lx)", stat );

		// --
		// -- Link Driver and Task

		if ( TASK_LOCK(tn) != LSTAT_Okay )
		{
			USBPANIC( "_Start_Function : Invalid TN %p", tn );
		}

		#ifdef DO_DEBUG

		if ( tn->tn_Owner )
		{
			USBPANIC( "_Start_Function : Already set tn_Owner  %p", tn->tn_Owner );
		}

		if ( dn->dn_Task )
		{
			USBPANIC( "_Start_Function : Already set dn_Task  %p", dn->dn_Task );
		}

		#endif

		tn->tn_Owner = dn;
		dn->dn_Task = tn;

		// --

		SEMAPHORE_OBTAIN(	& usbbase->usb_Bind_Semaphore );
		NODE_ADDTAIL(		& usbbase->usb_Bind_Header, dn );
		SEMAPHORE_RELEASE(	& usbbase->usb_Bind_Semaphore );

//		retval = stat;
	}
	else
	{
		USBDEBUG( "_Start_Function          : Failed (Stat $%08lx)", stat );

		if ( DRIVER_FREE( dn ) != FSTAT_Okay )
		{
			USBDEBUG( "_Start_Function          : Unable to Free, parking" );

			SEMAPHORE_OBTAIN(	& usbbase->usb_Bind_Semaphore );
			NODE_ADDTAIL(		& usbbase->usb_Bind_Header, dn );
			SEMAPHORE_RELEASE(	& usbbase->usb_Bind_Semaphore );
		}

		dn = NULL;

//		retval = stat;
	}

	// --

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --

static enum FDSTAT _Check_for_Functions( 
	struct USBBase *usbbase, 
	struct RealFunctionNode *fn, 
	struct USB2_ASync *as )
{
struct USB2_FktDriverNode *fdn;
enum FDSTAT stat;
U32 taskstat;

//	USBERROR( "_Check_for_Functions" );
	USBDEBUG( "fkt ............ : %p", fn );
	USBDEBUG( "fkt_Address .... : %ld", fn->fkt_Address );
	USBDEBUG( "fkt_Tier ....... : %ld", fn->fkt_Tier );
	USBDEBUG( "fkt_Class ...... : %ld", fn->fkt_Class );
	USBDEBUG( "fkt_SubClass ... : %ld", fn->fkt_SubClass );
	USBDEBUG( "fkt_Protocol ... : %ld", fn->fkt_Protocol );

	stat = FDSTAT_Not_Found;

	if ( ! fn->fkt_Owner )
	{
		fdn = Header_Head( & usbbase->usb_FDriver_Header );

		while( fdn )
		{
			while( fdn->fdn_Type == USB2Val_FDriver_Function )
			{
//				usbbase->usb_IExec->DebugPrintF( "FDN Check Function : %p\n", fdn );

				if ( fdn->fdn_Detach )
				{
//					USBERROR( "FKT : fdn_Detach" );
					break;
				}

				if (( fdn->fdn_Class != fn->fkt_Class ) && ( fdn->fdn_Class != FDCLASS_Any ))
				{
//					USBERROR( "FKT : fdn_Class : %ld : %ld :", fdn->fdn_Class, fn->fkt_Class );
					break;
				}

				if (( fdn->fdn_SubClass != fn->fkt_SubClass ) && ( fdn->fdn_SubClass != FDSUBCLASS_Any ))
				{
//					USBERROR( "FKT : fdn_SubClass : %ld : %ld :", fdn->fdn_SubClass, fn->fkt_SubClass );
					break;
				}

				if (( fdn->fdn_Protocol	!= fn->fkt_Protocol ) && ( fdn->fdn_Protocol != FDPROTOCOL_Any ))
				{
//					USBERROR( "FKT : fdn_Protocol : %ld : %ld :", fdn->fdn_Protocol, fn->fkt_Protocol );
					break;
				}

//				USBDEBUG( "Found Function [ possible : '%s' ]", (fdn->fdn_Title)?fdn->fdn_Title:"" );
//				USBERROR( "Found Function [ possible : '%s' ]", (fdn->fdn_Title)?fdn->fdn_Title:"" );

				taskstat = _Start_Function( usbbase, fn, fdn, as );

//				USBDEBUG( "_Start_Function return (Stat $%08lx)", taskstat );

				if (( taskstat == TASK_Return_Okay )
				||	( taskstat == TASK_Return_Stack_Okay ))
				{
//					USBDEBUG( "Found Function" );
//					USBERROR( "Found Function" );
					stat = FDSTAT_Found;
					break;
				}
				else 
				if (( taskstat == TASK_Return_Error )
				||	( taskstat == TASK_Return_Stack_Error ))
				{
					if ( fn->fkt_Tier == 0 )	// Tier 0 = RootHUB
					{
						USBDEBUG( "Function error (Stat $%08lx)", taskstat );
						stat = FDSTAT_Error;
						goto bailout;
					}
					else
					{
						// maybe do a Workbench notify in the future
						USBDEBUG( "Failed to start Function" );
					}
				}

				stat = FDSTAT_Not_Found;
				break;
			}

			if ( stat == FDSTAT_Found )
			{
				break;
			}

			fdn = Node_Next( fdn );
		}
	}

bailout:

	return( stat );
}

// --
