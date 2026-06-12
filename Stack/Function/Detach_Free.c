
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __EP( struct USBBase *usbbase, struct USB3_EndPointNode *ep )
{
//S32 remval;
S32 total;
//S32 max;
PTR ioreq;
//PTR next;

//	USBDEBUG( "__EP : %p\n", ep );

//	max		= 5;
	total	= 0;
//	remval	= 1;

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	// -- First 'try' to Abort Queued IORequests
	// Do this first so they do not get started
	// when the Active is aborted

	ioreq = ep->ep_Request_Queue.uh_Head;

	while( ioreq )
	{
//		USBDEBUG( "__EP : Abort %p [Queued]\n", ioreq );

		if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
		{
			SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
			IO_ABORT( ioreq );
			SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
			IOREQUEST_UNLOCK( ioreq );
		}

		ioreq = Node_Next( ioreq );
	}

	// -- 'try' to Abort Active IORequests

	ioreq = ep->ep_Request_Active;

	if ( ioreq )
	{
//		USBDEBUG( "__EP : Abort %p [Active]\n", ioreq );

		if ( IOREQUEST_LOCK( ioreq ) == LSTAT_Okay )
		{
			SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
			IO_ABORT( ioreq );
			SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
			IOREQUEST_UNLOCK( ioreq );
		}
	}

	// --

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

//	USBDEBUG( "__EP : %ld\n", total );

	return( total );
}

// --

SEC_CODE static S32 __IN( struct USBBase *usbbase, struct USB3_InterfaceNode *in )
{
struct USB3_EndPointNode *ep;
S32 remval;
S32 total;
PTR next;
S32 max;

//	USBDEBUG( "__IN : %p\n", in );

	max		= 5;
	total	= 0;
	remval	= 1;

	while(( remval > 0 ) && ( max-- > 0 ))
	{
		remval = 0;

		ep = in->in_EndPoints.uh_Head;

		while( ep )
		{
			next = Node_Next( ep );

			// --

			remval += __EP( usbbase, ep );

			// --

			NODE_REMNODE( & in->in_EndPoints, ep );

			if ( ENDPOINT_FREE(ep) != FSTAT_Okay )
			{
				NODE_ADDHEAD( & in->in_EndPoints, ep );
//				USBDEBUG( "__EP : Keep %p\n", ep );
			}
			else
			{
//				USBDEBUG( "__EP : Rem %p\n", ep );
				remval++;
			}

			// --

			ep = next;
		}

		total += remval;
	}

//	USBDEBUG( "__IN : %ld\n", total );

	return( total );
}

// --

SEC_CODE static S32 __IH( struct USBBase *usbbase, struct USB3_InterfaceHeader *ih )
{
struct USB3_InterfaceNode *in;
S32 remval;
S32 total;
PTR next;
S32 max;

//	USBDEBUG( "__IH : %p\n", ih );

	max		= 5;
	total	= 0;
	remval	= 1;

	while(( remval > 0 ) && ( max-- > 0 ))
	{
		remval = 0;

		in = ih->ih_AltSettings.uh_Head;

		while( in )
		{
			next = Node_Next( in );

			// --

			remval += __IN( usbbase, in );

			// --

			NODE_REMNODE( & ih->ih_AltSettings, in );

			if ( INTERFACE_FREENODE( in ) != FSTAT_Okay )
			{
				NODE_ADDHEAD( & ih->ih_AltSettings, in );
//				USBDEBUG( "__IN : Keep %p\n", in );
			}
			else
			{
//				USBDEBUG( "__IN : Rem %p\n", in );
				remval++;
			}

			// --

			in = next;
		}

		total += remval;
	}

//	USBDEBUG( "__IH : %ld\n", total );

	return( total );
}

// --

SEC_CODE static S32 __IG( struct USBBase *usbbase, struct USB3_InterfaceGroup *ig )
{
struct USB3_InterfaceHeader *ih;
S32 remval;
S32 total;
PTR next;
S32 max;

//	USBDEBUG( "__IG : %p\n", ig );

	max		= 5;
	total	= 0;
	remval	= 1;

	while(( remval > 0 ) && ( max-- > 0 ))
	{
		remval = 0;

		ih = ig->ig_Group.uh_Head;

		while( ih )
		{
			next = Node_Next( ih );

			// --

			remval += __IH( usbbase, ih );

			// --

			NODE_REMNODE( & ig->ig_Group, ih );

			if ( INTERFACE_FREEHEADER(ih) != FSTAT_Okay )
			{
				NODE_ADDHEAD( & ig->ig_Group, ih );
//				USBDEBUG( "__IH : Keep %p\n", ih );
			}
			else
			{
//				USBDEBUG( "__IH : Rem %p\n", ih );
				remval++;
			}

			// --

			ih = next;
		}

		total += remval;
	}

//	USBDEBUG( "__IG : %ld\n", total );

	return( total );
}

// --

SEC_CODE static S32 __CN( struct USBBase *usbbase, struct USB3_ConfigNode *cn )
{
struct USB3_InterfaceGroup *ig;
S32 remval;
S32 total;
PTR next;
S32 max;

//	USBDEBUG( "__CN : %p\n", cn );

	max		= 5;
	total	= 0;
	remval	= 1;

	while(( remval > 0 ) && ( max-- > 0 ))
	{
		remval = 0;

		ig = cn->cfg_InterfaceGroups.uh_Head;

		while( ig )
		{
			next = Node_Next( ig );

			// --

			remval += __IG( usbbase, ig );

			// --

			NODE_REMNODE( & cn->cfg_InterfaceGroups, ig );

			if ( INTERFACE_FREEGROUP(ig) != FSTAT_Okay )
			{
				NODE_ADDHEAD( & cn->cfg_InterfaceGroups, ig );
//				USBDEBUG( "__IG : Keep %p\n", ig );
			}
			else
			{
//				USBDEBUG( "__IG : Rem %p\n", ig );
				remval++;
			}

			// --

			ig = next;
		}

		total += remval;
	}

//	USBDEBUG( "__CN : %ld\n", total );

	return( total );
}

// --

/*
** Everything should have been marked for Detach now.
**
** Lets see if we can release some resources
*/

void __Function_FreeDetach( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
struct USB3_ConfigNode *cn;
S32 remval;
S32 total;
PTR next;
S32 max;

//	USBDEBUG( "__Function_FreeDetach\n" );

//	usbbase->usb_IExec->DebugPrintF( "!!!!!!!!!!!!!!!!!!!!!!!!! __Function_FreeDetach\n\n\n" );

	total = 0;

	if ( FUNCTION_LOCK( fn ) == LSTAT_Okay )
	{
		max = 5;
		remval = 1;

		while(( remval > 0 ) && ( max-- > 0 ))
		{
			remval = 0;

			cn = fn->fkt_Configs.uh_Head;
			
			while(cn)
			{
				next = Node_Next( cn );

				// --

				remval += __CN( usbbase, cn );

				// --

				NODE_REMNODE( & fn->fkt_Configs, cn );

				if ( CONFIG_FREE(cn) != FSTAT_Okay )
				{
					NODE_ADDHEAD( & fn->fkt_Configs, cn );

					USBDEBUG( "__Function_FreeDetach    : Failed to removed CN %p", cn );
				}
				else
				{
					USBDEBUG( "__Function_FreeDetach : Removed CN %p\n", cn );

					if ( fn->fkt_Config_Active == cn )
					{
						USBDEBUG( "__Function_FreeDetach : Clearing fkt_Config_Active %p", cn );
						fn->fkt_Config_Active = NULL;
					}

					remval++;
				}

				// --

				cn = next;
			}

			total += remval;
		}

		FUNCTION_UNLOCK( fn );
	}

//	USBDEBUG( "__Function_FreeDetach : %ld\n", total );
}

// --
