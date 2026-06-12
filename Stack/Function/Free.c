
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

SEC_CODE static void __Release( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
struct USB3_InterfaceHeader *ih2;
struct USB3_InterfaceHeader *ih;
struct USB3_InterfaceGroup *ig2;
struct USB3_InterfaceGroup *ig;
struct USB3_InterfaceNode *in2;
struct USB3_InterfaceNode *in;
struct USB3_EndPointNode *ep2;
struct USB3_EndPointNode *ep;
struct USB3_ConfigNode *cn2;
struct USB3_ConfigNode *cn;
struct RealRequest *ioreq;

	USBDEBUG( "__Release                : FN    %p : 1", fn );

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	USBDEBUG( "__Release                : FN    %p : 2", fn );

	// --

	USBDEBUG( "fn : %p, Locks %ld", fn, fn->fkt_Locks );

	cn = fn->fkt_Configs.uh_Head;

	USBDEBUG( "cn : %p, Locks %ld", cn, (cn)?cn->cfg_Locks:0 );

	while( cn )
	{
		USBDEBUG( "cn : %p, Locks %ld", cn, cn->cfg_Locks );

		cn2 = Node_Next( cn );

		// --

		ig = cn->cfg_InterfaceGroups.uh_Head;

		USBDEBUG( "ig : %p, Locks %ld", ig, ig->ig_Locks );

		while( ig )
		{
			USBDEBUG( "ig : %p, Locks %ld", ig, ig->ig_Locks );

			ig2 = Node_Next( ig );

			// --

			ih = ig->ig_Group.uh_Head;

			USBDEBUG( "ih : %p, Locks %ld", ih, ih->ih_Locks );

			while( ih )
			{
				USBDEBUG( "ih : %p, Locks %ld", ih, ih->ih_Locks );

				ih2 = Node_Next( ih );

				// --

				in = ih->ih_AltSettings.uh_Head;

				USBDEBUG( "in : %p, Locks %ld", in, in->in_Locks );

				while( in )
				{
					USBDEBUG( "in : %p, Locks %ld", in, in->in_Locks );

					in2 = Node_Next( in );

					// --

					ep = in->in_EndPoints.uh_Head;

					USBDEBUG( "ep : %p, Locks %ld", ep, ep->ep_Locks );

					while( ep )
					{
						USBDEBUG( "ep : %p, Locks %ld", ep, ep->ep_Locks );

						ep2 = Node_Next( ep );

						// --

						if ( ENDPOINT_LOCK(ep) == LSTAT_Okay )
						{
							// --

							while( TRUE )
							{
								ioreq = NODE_REMHEAD( & ep->ep_Request_Queue );

								USBDEBUG( "ioreq : %p, Locks %ld", ioreq, (ioreq)?ioreq->req_Locks:0 );

								if ( ! ioreq )
								{
									break;
								}

								SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
								IO_STOP( ioreq );
								SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
							}

							// --

							ioreq = ep->ep_Request_Active;

							USBDEBUG( "ioreq : %p, Locks %ld", ioreq, (ioreq)?ioreq->req_Locks:0 );

							if ( ioreq )
							{
								SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
								IO_STOP( ep->ep_Request_Active );
								SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
							}

							// --

							ENDPOINT_UNLOCK(ep);

							// --

							NODE_REMNODE( & in->in_EndPoints, ep );

							USBDEBUG( "ep : %p, Locks %ld", ep, ep->ep_Locks );

							if ( ENDPOINT_FREE(ep) != FSTAT_Okay )
							{
								NODE_ADDHEAD( & in->in_EndPoints, ep );
								USBDEBUG( "ep : %p - NOT Freed", ep );
							}
							else
							{
								USBDEBUG( "ep : %p - Freed", ep );
							}

							// --

							ep = ep2;
						}
					}

					// --

					NODE_REMNODE( & ih->ih_AltSettings, in );

					USBDEBUG( "in : %p, Locks %ld", in, in->in_Locks );

					if ( INTERFACE_FREENODE( in ) != FSTAT_Okay )
					{
						NODE_ADDHEAD( & ih->ih_AltSettings, in );
						USBDEBUG( "in : %p - NOT Freed", in );
					}
					else
					{
						USBDEBUG( "in : %p - Freed", in );
					}

					// --

					in = in2;
				}

				// --

				NODE_REMNODE( & ig->ig_Group, ih );

				USBDEBUG( "ih : %p, Locks %ld", ih, ih->ih_Locks );

				if ( INTERFACE_FREEHEADER(ih) != FSTAT_Okay )
				{
					NODE_ADDHEAD( & ig->ig_Group, ih );
					USBDEBUG( "ih : %p - NOT Freed", ih );
				}
				else
				{
					USBDEBUG( "ih : %p - Freed", ih );
				}

				// --

				ih = ih2;
			}

			// --

			NODE_REMNODE( & cn->cfg_InterfaceGroups, ig );

			USBDEBUG( "ig : %p, Locks %ld", ig, ig->ig_Locks );

			if ( INTERFACE_FREEGROUP(ig) != FSTAT_Okay )
			{
				NODE_ADDHEAD( & cn->cfg_InterfaceGroups, ig );
				USBDEBUG( "ig : %p - NOT Freed", ig );
			}
			else
			{
				USBDEBUG( "ig : %p - Freed", ig );
			}

			// --

			ig = ig2;
		}

		// --

		NODE_REMNODE( & fn->fkt_Configs, cn );

		USBDEBUG( "cn : %p, Locks %ld", cn, cn->cfg_Locks );

		if ( CONFIG_FREE(cn) != FSTAT_Okay )
		{
			NODE_ADDHEAD( & fn->fkt_Configs, cn );

			USBDEBUG( "cn : %p - NOT Freed", cn );
		}
		else
		{
			USBDEBUG( "cn : %p - Freed", cn );

			if ( fn->fkt_Config_Active == cn )
			{
				USBDEBUG( "__Release : Clearing fkt_Config_Active %p", cn );
				fn->fkt_Config_Active = NULL;
			}
		}

		// --

		cn = cn2;
	}

	// --

	ep = fn->fkt_ControlEndPoint;

	USBDEBUG( "ep ctrl : %p, Locks %ld", ep, (ep)?ep->ep_Locks:0 );

	if ( ep )
	{
		if ( ENDPOINT_FREE( ep ) == FSTAT_Okay )
		{
			fn->fkt_ControlEndPoint = NULL;
			USBDEBUG( "ep ctrl : %p - Freed", ep );
		}
		else
		{
			USBDEBUG( "ep ctrl : %p - NOT Freed", ep );
		}
	}	

	// --

	if (( fn->fkt_Address > 0 ) && ( fn->fkt_Address < 128 ))
	{
		HCD_ADDR_RELEASE( fn );
	}

	// --

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

/*
** The Node, should have been Removed from the list header before calling.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __Function_Free( struct USBBase *usbbase, struct RealFunctionNode *fn, STR file UNUSED )

#else

SEC_CODE enum FSTAT __Function_Free( struct USBBase *usbbase, struct RealFunctionNode *fn )

#endif

{
struct USB3_HCDNode *hn;
enum VSTAT vstat;
enum FSTAT fstat;

	USBDEBUG( "__Function_Free           : FN    %p : (%s)", fn, (file)?file:"<NULL>" );
//	USBERROR( "__Function_Free           : FN    %p : (%s)", fn, (file)?file:"<NULL>" );
//	usbbase->usb_IExec->DebugPrintF( "!!!!!!!!!!!!!!!!!!!!!!!!! __Function_Free\n\n\n" );

	// --

	if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
	{
		if ( ! fn->fkt_FreeMe )
		{
			USBINFO( "Marking FN %p FreeMe", fn );
			fn->fkt_FreeMe = TRUE;
		}

		if ( ! fn->fkt_ActiveIOReq )
		{
//			usbbase->usb_IExec->DebugPrintF( "FN %p : fkt_ActiveIOReq %ld : Freeing\n", fn, fn->fkt_ActiveIOReq );
			__Release( usbbase, fn );
		}
//		else
//		{
//			usbbase->usb_IExec->DebugPrintF( "FN %p : fkt_ActiveIOReq %ld : Skipping\n", fn, fn->fkt_ActiveIOReq );
//		}

		FUNCTION_UNLOCK( fn );
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = FUNCTION_VALID(fn);

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if ( fn->fkt_Locks > 0 )
		{
			USBDEBUG( "__Function_Free           : FN    %p : Function Node still have %ld locks", fn, fn->fkt_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			fn->fkt_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__Function_Free           : FN    %p : Invalid Pointer", fn );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__Function_Free           : Freeing %p : (%s)", fn, (file)?file:"<NULL>" );

	// --

	#ifdef DO_PANIC

	if ((( Node_Next( & fn->fkt_DriverNode )) && ( Node_Next( & fn->fkt_DriverNode ) != (PTR) 0xAC111111 ))
	||	(( Node_Prev( & fn->fkt_DriverNode )) && ( Node_Prev( & fn->fkt_DriverNode ) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__Function_Free           : Function Node have not been removed : Self %p : Next %p : Prev %p : (%s)", fn, Node_Next( & fn->fkt_DriverNode ), Node_Prev( & fn->fkt_DriverNode ), file );
	}

	#endif

	// --

	hn = fn->fkt_HCD;

	if ( hn )
	{
		fn->fkt_HCD = NULL;
		HCD_UNLOCK( hn );
	}

	// --

	#if 0

	if ( fn->fkt_ProcessName )
	{
		MEM_FREEVEC( fn->fkt_ProcessName );
		fn->fkt_ProcessName = NULL;
	}

	#endif

	if ( fn->fkt_HUBDescriptor )
	{
		MEM_FREEIOBUFFER( fn->fkt_HUBDescriptor );
		fn->fkt_HUBDescriptor = NULL;
	}

	if ( fn->fkt_DeviceDescriptor )
	{
		MEM_FREEIOBUFFER( fn->fkt_DeviceDescriptor );
		fn->fkt_DeviceDescriptor = NULL;
	}

	if ( fn->fkt_ASync )
	{
		ASYNC_SUB( fn->fkt_ASync );
		fn->fkt_ASync = NULL;
	}

	SEMAPHORE_OBTAIN(	& usbbase->usb_Fkt_Semaphore );
	NODE_REMNODE(		& usbbase->usb_Fkt_Header, & fn->fkt_GlobalNode );
	SEMAPHORE_RELEASE(	& usbbase->usb_Fkt_Semaphore );

	MEMORY_FREE( MEMID_USBFunction, fn, 0 );

bailout:

	return( fstat );
}

// --
