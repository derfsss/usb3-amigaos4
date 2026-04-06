
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb2_all.h"

// --

SEC_CODE static void __myIFC_Entry( struct USBBase *usbbase, PTR userdata, PTR in UNUSED )
{
struct USB2DriverIFace *ifc;
struct USB2_DriverNode *dn;
PTR base;
S32 proc;
S32 old;

	TASK_NAME_ENTER( "__myIFC_Entry" );

	dn = userdata;

//	USBDEBUG( "__myIFCT_Entry : 1 : DN    %p : Call Driver", dn );

	// --

	if ( DRIVER_LOCK( dn ) == LSTAT_Okay )
	{
		/**/ if ( dn->dn_Entry )
		{
//			USBERROR( "__myIFC_Entry : 2 : DN    %p : Entry %p", dn, dn->dn_Entry );

			dn->dn_Entry( usbbase, & dn->dn_Message.rdm_Public );
		}
		else if (( dn->dn_Filename ) && ( usbbase->usb_DriverDirLock ))
		{
//			USBERROR( "__myIFC_Entry : 3 : DN    %p : Filename '%s'", dn, dn->dn_Filename );

			proc = MISC_SETPROCWINDOW( -1 );

//			USBERROR( "lock : $%08lx", usbbase->usb_DriverDirLock );

			old  = MISC_SETCURRENTDIR( usbbase->usb_DriverDirLock );

//			USBERROR( "old  : $%08lx", old );

			base = MISC_OPENLIBRARY( dn->dn_Filename, 0 );

			USBERROR( "base : %p", base );

			ifc	 = MISC_OBTAININTERFACE( base, "main", 1 );

			USBERROR( "ifc  : %p", ifc );

			MISC_SETPROCWINDOW( proc );

			if ( ifc )
			{
				ifc->Entry( & dn->dn_Message.rdm_Public );
			}

			MISC_RELEASEINTERFACE( ifc );

			MISC_CLOSELIBRARY( base );

			MISC_SETCURRENTDIR( old );
		}
		else
		{
			USBERROR( "__myIFC_Entry : 4 : DN    %p : No Entry", dn );
		}

		// Driver Quit
		dn->dn_FreeMe = TRUE;
	
		DRIVER_UNLOCK( dn );
	}

	// --

//	USBERROR( "__myIFC_Entry : 5 : DN    %p : Driver Exit", dn );

	TASK_NAME_LEAVE();
}

// --

SEC_CODE static S32 _Start_Interface( 
	struct USBBase *usbbase, 
	struct RealFunctionNode *fn,
	struct USB2_FktDriverNode *fdn,
	struct USB2_InterfaceGroup *ig, 
	struct USB2_ASync *as )
{
struct USB2_InterfaceHeader *ih;
struct USB2_DriverNode *dn;
struct USB2_TaskNode *tn;
enum TaskReturn stat;
U32 retval;

	TASK_NAME_ENTER( "_Start_Interface" );

	USBDEBUG( "_Start_Interface" );

	retval = TASK_Return_Stack_Error;

	dn = DRIVER_ALLOC( fn, as );

	if ( ! dn )
	{
		USBERROR( "_Start_Interface : Error allocating memory" );
		goto bailout;
	}

	dn->dn_Entry = fdn->fdn_Entry;
	dn->dn_Filename = fdn->fdn_Driver_Filename;

	// -- Init Public Message

	// Alloc check FN
//	if ( FUNCTION_VALID(fn) != VSTAT_Okay )
//	{
//		USBERROR( "_Start_Interface : Function Valid error (%p)", fn );
//		goto bailout;
//	}

	dn->dn_Message.rdm_Public.IUSB2 = usbbase->usb_IUSB2;
	dn->dn_Message.rdm_Public.Function = (PTR) fn;
	dn->dn_Message.rdm_Public.ConfigDescriptors = (PTR) fn->fkt_Config_Desc_Buf;
	
	// --

	if ( INTERFACE_VALIDGROUP(ig) != VSTAT_Okay )
	{
		USBERROR( "_Start_Interface : Interface Group Valid error (%p)", ig );
		goto bailout;
	}

	ih = ig->ig_Group.uh_Head;

	if ( INTERFACE_VALIDHEADER(ih) != VSTAT_Okay )
	{
		USBERROR( "_Start_Interface : Interface Header Valid error (%p)", ih );
		goto bailout;
	}

	#ifdef DO_DEBUG

	if ( ! ih->ih_Active )
	{
		USBPANIC( "_Start_Interface : ih_Active not set" );
	}

	#endif

	if ( ih->ih_Owner )
	{
		// Allready claimed
//		USBERROR( "Skipping Interface (claimed) : FN    %p : Class %ld", ig->ig_Function, ig->ig_Class );
		goto bailout;
	}

//	USBERROR( "Found Interface (possible) : FN    %p : Class %ld", ig->ig_Function, ig->ig_Class );

	dn->dn_Message.rdm_Public.Interface = (PTR) & ih->ih_Public ;
	dn->dn_Message.rdm_Public.InterfaceDescriptor = (PTR) ih->ih_Active->in_Descriptor;

	// -- Start Driver

	stat = TASK_START(
		TASK_Tag_Func_Main, __myIFC_Entry,
		TASK_Tag_UserData, dn,
		TASK_Tag_Get_TN, & tn,
		TASK_Tag_Prioity, fdn->fdn_Pri,
		TASK_Tag_ASync, as,
		TASK_Tag_Type, TASK_Type_FKT,
		TASK_Tag_Name, fdn->fdn_Title,
		TAG_END	
	);

	// --

	retval = stat;

	if ( TASKRETURN_OK(stat) )
	{
		USBDEBUG( "_Start_Interface         : Okay (Stat $%08lx)", stat );

		// --
		// -- Link Driver and Task

		if ( TASK_LOCK(tn) != LSTAT_Okay )
		{
			USBPANIC( "_Start_Interface : Invalid TN %p", tn );
		}

		#ifdef DO_DEBUG

		if ( tn->tn_Owner )
		{
			USBPANIC( "_Start_Interface : Already set tn_Owner  %p", tn->tn_Owner );
		}

		if ( dn->dn_Task )
		{
			USBPANIC( "_Start_Interface : Already set dn_Task  %p", dn->dn_Task );
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
		USBERROR( "_Start_Interface         : Failed (Stat $%08lx)", stat );

		if ( DRIVER_FREE( dn ) != FSTAT_Okay )
		{
			USBDEBUG( "_Start_Interface         : Unable to Free, parking" );

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

SEC_CODE static S32 _Check_Interface( 
	struct USBBase *usbbase, 
	struct RealFunctionNode *fn, 
	struct USB2_InterfaceGroup *ig, 
	struct USB2_ASync *as )
{
struct USB2_FktDriverNode *fdn;
S32 retval;
S32 stat;

//	USBERROR( "_Check_Interface" );

	retval = FALSE;

	// todo should there be a IfcGroup Ower??
	// and check here?
	// ih_Owner .. there is a IfcHeader owner

	fdn = Header_Head( & usbbase->usb_FDriver_Header );

	usbbase->usb_IExec->DebugPrintF( "USB: _CheckIfc: first fdn=%p ig_class=%ld\n", fdn, (U32) ig->ig_Class );

	while( fdn )
	{
		usbbase->usb_IExec->DebugPrintF( "USB: _CheckIfc: fdn=%p type=%ld class=%ld title=%s\n",
			fdn, (U32) fdn->fdn_Type, (U32) fdn->fdn_Class,
			fdn->fdn_Title ? fdn->fdn_Title : "(null)" );

		while( fdn->fdn_Type == USB2Val_FDriver_Interface )
		{
			usbbase->usb_IExec->DebugPrintF( "USB: _CheckIfc match: fdn class=%ld sub=%ld proto=%ld vs ig class=%ld sub=%ld proto=%ld\n",
				(U32) fdn->fdn_Class, (U32) fdn->fdn_SubClass, (U32) fdn->fdn_Protocol,
				(U32) ig->ig_Class, (U32) ig->ig_SubClass, (U32) ig->ig_Protocol );
//			usbbase->usb_IExec->DebugPrintF( "FDN Check Interface : %p\n", fdn );

			if ( fdn->fdn_Detach )
			{
				USBERROR( "IFC : fdn_Detach" );
				break;
			}

			if (( fdn->fdn_Class != ig->ig_Class ) && ( fdn->fdn_Class != FDCLASS_Any ))
			{
//				USBERROR( "IFC : fdn_Class : %ld : %ld :", fdn->fdn_Class, ig->ig_Class );
				break;
			}

			if (( fdn->fdn_SubClass != ig->ig_SubClass ) && ( fdn->fdn_SubClass != FDSUBCLASS_Any ))
			{
//				USBERROR( "IFC : fdn_SubClass : %ld : %ld :", fdn->fdn_SubClass, ig->ig_SubClass );
				break;
			}

			if (( fdn->fdn_Protocol	!= ig->ig_Protocol ) && ( fdn->fdn_Protocol != FDPROTOCOL_Any ))
			{
//				USBERROR( "IFC : fdn_Protocol : %ld : %ld :", fdn->fdn_Protocol, ig->ig_Protocol );
				break;
			}

//			USBDEBUG( "Found Interface [ possible : '%s' ]", (fdn->fdn_Title)?fdn->fdn_Title:"" );
//			USBERROR( "Found Interface [ possible : '%s' ]", (fdn->fdn_Title)?fdn->fdn_Title:"" );

			usbbase->usb_IExec->DebugPrintF( "USB: _CheckIfc: MATCH! Starting '%s'\n",
				fdn->fdn_Title ? fdn->fdn_Title : "(null)" );

			stat = _Start_Interface( usbbase, fn, fdn, ig, as );

			usbbase->usb_IExec->DebugPrintF( "USB: _CheckIfc: _Start_Interface returned %ld\n", (S32) stat );

			if ( myIS_TASKRETURN_ERR( stat ))
			{
//				USBERROR( "Skipping Interface : Stat $%08lx", stat );
				break;
			}

//			USBDEBUG( "Found Interface" );
//			USBERROR( "Found Interface : Stat $%08lx", stat );

			retval = TRUE;
			break;
		}

		if ( retval )
		{
			break;
		}

		fdn = Node_Next( fdn );
	}

	return( retval );
}

// --

static enum FDSTAT _Check_for_Interfaces( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as )
{
//struct USB2_InterfaceHeader *ih;
struct USB2_InterfaceGroup *ig;
struct USB2_ConfigNode *cn;
enum FDSTAT stat;

	USBDEBUG( "_Check_for_Interfaces: 1" );

	stat = FDSTAT_Not_Found;

	cn = fn->fkt_Config_Active;

	usbbase->usb_IExec->DebugPrintF( "USB: CheckInterfaces: cfgActive=%p addr=%ld\n", cn, (U32) fn->fkt_Address );

	if ( cn )
	{
		ig = cn->cfg_InterfaceGroups.uh_Head;

		usbbase->usb_IExec->DebugPrintF( "USB: CheckInterfaces: first ig=%p\n", ig );

		while( ig )
		{
			usbbase->usb_IExec->DebugPrintF( "USB: CheckInterfaces: ig=%p class=%ld sub=%ld proto=%ld\n",
				ig, (U32) ig->ig_Class, (U32) ig->ig_SubClass, (U32) ig->ig_Protocol );

			if ( _Check_Interface( usbbase, fn, ig, as ))
			{
				stat = FDSTAT_Found;
			}

			ig = Node_Next( ig );
		}
	}

	return( stat );
}

// --
