
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
// Release all resource we can, for this Struct

SEC_CODE static void __Release( struct USBBase *usbbase, struct RealRegister *reg )
{
//struct RealEndPointResource *real;
struct RealEndPointResource *next;
struct RealEndPointResource *epr;
struct USB2_InterfaceHeader *ih;
struct USB2_InterfaceGroup *ig;
struct RealFunctionNode *fn;
struct USB2_ConfigNode *cn;

//	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	// --
	// Free EndPoint Resources

	epr = reg->reg_EPRHeader.uh_Head;

	USBDEBUG( "__Register_Free          : Trying to free EPR %p (1)", epr );

	while( epr )
	{
		USBDEBUG( "__Register_Free          : Trying to free EPR %p (2)", epr );

		next = Node_Next( epr );
		// --

//		real = (PTR) ( (char *) epr - offsetof( struct RealEndPointResource, epr_Node ));

//		#ifdef DO_DEBUG
//
//		if ( ENDPOINTRES_VALID(epr) != VSTAT_Okay )
//		{
//			USBPANIC( "__Register_Free : Invalid EPR : %p -> %p", epr, real );
//		}
//
//		#endif

		USBDEBUG( "__Register_Free          : Trying to free EPR %p", epr );
		USBDEBUG( "--> : REM 4 : EPR %p to REG %p : Title '%s' :", epr, reg, reg->reg_Title );
		NODE_REMNODE( & reg->reg_EPRHeader, & epr->epr_Node );

		if ( ENDPOINTRES_FREE( epr ) != FSTAT_Okay )
		{
			USBDEBUG( "__Register_Free : Failed to free EPR %p", epr );
			USBDEBUG( "--> : ADD 5 : EPR %p to REG %p : Title '%s' :", epr, reg, reg->reg_Title );
			NODE_ADDTAIL( & reg->reg_EPRHeader, & epr->epr_Node );
		}
		else
		{
			USBDEBUG( "__Register_Free          : Free'd EPR %p", epr );
			if ( reg->reg_Public.Res_Control == & epr->epr_Public )
			{
				USBDEBUG( "__Register_Free          : Found Control EPR (1)" );
				reg->reg_Public.Res_Control = NULL;
			}
		}
		// --
		epr = next;
	}

	// -- Release Claims and Function Lock

	fn = reg->reg_Function;

	if (( fn ) && ( FUNCTION_VALID(fn) == VSTAT_Okay ))
	{
		// Again only Active Config ??
 		cn = fn->fkt_Config_Active;

 		if (( cn ) && ( CONFIG_VALID(cn) == VSTAT_Okay ))
 		{
 			ig = cn->cfg_InterfaceGroups.uh_Head;

 			while( ig )
 			{
 				ih = ig->ig_Group.uh_Head;

 				while( ih )
 				{
 					if ( ih->ih_Owner == reg )
 					{
						INTERFACE_DECLAIMHEADER( reg, ih );
 					}

//	reg_EPRHeader

 					ih = Node_Next( ih );
 				}

 				ig = Node_Next( ig );
 			}
 		}

		if ( fn->fkt_Owner == reg )
		{
			FUNCTION_DECLAIM( reg, fn );
		}

		FUNCTION_UNLOCK( fn );
		reg->reg_Function = NULL;
	}

	// --

	ih = reg->reg_Interface;

	if (( ih ) && ( INTERFACE_VALIDHEADER(ih) == VSTAT_Okay ))
	{
		INTERFACE_UNLOCKHEADER(ih);
		reg->reg_Interface = NULL;
	}

	// --

//	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __Register_Free( struct USBBase *usbbase, struct RealRegister *reg, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Register_Free( struct USBBase *usbbase, struct RealRegister *reg )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;


	TASK_NAME_ENTER( "__Register_Free" );

//	USBERROR( "__Register_Free          : REG   %p : (%s)", reg, (file)?file:"<NULL>" );

	// --

	if ( REGISTER_LOCK( reg ) == LSTAT_Okay )
	{
		if ( ! reg->reg_FreeMe )
		{
			USBINFO( "Marking REG %p FreeMe", reg );
			reg->reg_FreeMe = TRUE;
		}

		__Release( usbbase, reg );

		REGISTER_UNLOCK( reg );
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = REGISTER_VALID( reg );

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( reg->reg_Locks > 0 )
		{
			USBINFO( "__Register_Free          : Register Node still have %ld locks", reg->reg_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			reg->reg_StructID = ID_USB2_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__Register_Free  : Invalid Pointer %p : (%s)", reg, file );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Register_Free          : Freeing %p : (%s)", reg, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next( reg )) && ( Node_Next( reg ) != (PTR) 0xAC111111 ))
	||	(( Node_Prev( reg )) && ( Node_Prev( reg ) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__Register_Free          : REGister Node have not been removed : Self %p : Next %p : Prev %p : (%s)", reg, Node_Next( reg ), Node_Prev( reg ), file );
	}

	#endif

	// --

	MSGPORT_FREE( & reg->reg_MsgPort );

	MEMORY_FREE( MEMID_USBRegister, reg, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
