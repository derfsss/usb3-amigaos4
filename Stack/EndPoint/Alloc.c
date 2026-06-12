
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

SEC_CODE struct USB3_EndPointNode *__EndPoint_Alloc(
	struct USBBase *usbbase,
	struct RealFunctionNode *fn,
	struct USB3_InterfaceNode *in,
	struct USB3_EndPoint_Desc *epdsc,
	STR file UNUSED )

#else

SEC_CODE struct USB3_EndPointNode *__EndPoint_Alloc(
	struct USBBase *usbbase,
	struct RealFunctionNode *fn,
	struct USB3_InterfaceNode *in,
	struct USB3_EndPoint_Desc *epdsc )

#endif

{
struct USB3_EndPointNode *ep;
U32 error;
U32 dir;

	USBDEBUG( "__EndPoint_Alloc         : (%s)", (file)?file:"<NULL>" );

	error = TRUE;

	ep = MEMORY_ALLOC( MEMID_USBEndPoint, TRUE );

	if ( ! ep )
	{
		USBERROR( "__EndPoint_Alloc : Error allocating memory" );
		goto bailout;
	}

	ep->ep_StructID			= ID_USB3_EP;
	ep->ep_Number			= EPADR_Dir_Number( epdsc->EndPointAddress );
	ep->ep_Type				= epdsc->Attributes & EPATT_Type_Mask;
	ep->ep_Descriptor		= epdsc;
	ep->ep_MaxPacketSize	= LE_SWAP16( epdsc->MaxPacketSize );
	ep->ep_Interval			= epdsc->Interval;
	ep->ep_DataToggle		= FALSE;

	dir = epdsc->EndPointAddress & ( EPADR_EndPoint_Mask | EPADR_Dir_Mask );

	/**/ if ( ! dir )
	{
		// Control is Always On, Other EndPoints start off
		ep->ep_Direction = EPDIR_Both;
		ep->ep_Active	 = TRUE;
	}
	else if (( dir & EPADR_Dir_Mask ) == EPADR_Dir_In )
	{
		ep->ep_Direction = EPDIR_In;
	}
	else
	{
		ep->ep_Direction = EPDIR_Out;
	}

	if ( FUNCTION_LOCK( fn ) != LSTAT_Okay )
	{
		USBERROR( "__EndPoint_Alloc : Function Lock Error" );
		goto bailout;
	}

	ep->ep_Function = fn;

	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBERROR( "__EndPoint_Alloc : Error Funtion Detached" );
		goto bailout;
	}

	// Interface Not given for Control EndPoints

	if ( in )	
	{
		if ( INTERFACE_LOCKNODE( in ) != LSTAT_Okay )
		{
			USBERROR( "__EndPoint_Alloc : Error Failed to lock Ifc" );
			goto bailout;
		}

		ep->ep_Interface = in;
	}

	// --

	error = FALSE;

bailout:

	if ((ep) && ( error ))
	{
		ENDPOINT_FREE(ep);
		ep = NULL;
	}

	USBDEBUG( "__EndPoint_Alloc         : EP    %p : (new)", ep );

	return( ep );
}

// --
