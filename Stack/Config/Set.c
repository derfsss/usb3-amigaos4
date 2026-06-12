
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static void __Update( struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB3_ConfigNode *cfgnode )
{
struct USB3_InterfaceHeader *ih;
struct USB3_Interface_Desc *ifcdsc;
struct USB3_InterfaceGroup *ig;
struct USB3_InterfaceNode *in;
struct USB3_EndPointNode **array;
struct USB3_EndPointNode *ep;
struct USB3_ConfigNode *cn;
struct RealRequest *ioreq;
U32 offset;

	SEMAPHORE_OBTAIN( & fn->fkt_Semaphore );

	// From now on the Device has switched Settings
	// so we have to force a config change...

	cn = fn->fkt_Config_Active;

	if ( cn )
	{
		ig = cn->cfg_InterfaceGroups.uh_Head;
		
		while( ig )
		{
			ih = ig->ig_Group.uh_Head;

			while( ih )
			{
				in = ih->ih_Active;

				if ( in )
				{
					ep = in->in_EndPoints.uh_Head;

					while( ep )
					{
						// Mark EndPoint as Disabled
						ep->ep_Active = FALSE;

						// Clear Queued IORequest first
						// Aborting the active will result in starting next queued
						while( TRUE )
						{
							ioreq = ep->ep_Request_Queue.uh_Head;

							if ( ! ioreq )
							{
								break;
							}

							IO_STOP( ioreq );
						}

						if ( ep->ep_Request_Active )
						{
							IO_STOP( ep->ep_Request_Active );
						}

						ep = Node_Next( ep );
					}
				}

				ih = Node_Next( ih );
			}

			ig = Node_Next( ig );
		}
	}

	// --

	#ifdef DO_DEBUG

	if ( fn->fkt_Address > 127 )
	{
		USBPANIC( "__Update : Invalid Funktion Address #%lu", fn->fkt_Address );
	}

	#endif

	array	= fn->fkt_HCD->hn_EndPoint_Array;
	offset	= fn->fkt_Address * 32;

	MEM_SET( & array[offset], 0, 32*4 );

	array[offset+ 0] = fn->fkt_ControlEndPoint;
	array[offset+16] = fn->fkt_ControlEndPoint;

	// --

	fn->fkt_Config_Active = cfgnode;

	ig = cfgnode->cfg_InterfaceGroups.uh_Head;

	while( ig )
	{
		ih = ig->ig_Group.uh_Head;

		while( ih )
		{
			in		= ih->ih_AltSettings.uh_Head;
			ifcdsc	= in->in_Descriptor;

			ih->ih_Active		= in;
			ih->ih_AltNumber	= ifcdsc->AlternateSetting;
			ih->ih_Owner		= NULL;

			ep = in->in_EndPoints.uh_Head;

			while( ep )
			{
				ep->ep_Active = TRUE;

				if ( ep->ep_Direction == EPDIR_Out )
				{
					array[ offset + ep->ep_Number + 16 ] = ep;
				}
				else
				{
					array[ offset + ep->ep_Number +  0 ] = ep;
				}

				ep = Node_Next( ep );
			}

			ih = Node_Next( ih );
		}

		ig = Node_Next( ig );
	}

	SEMAPHORE_RELEASE( & fn->fkt_Semaphore );
}

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE S32 __Config_Set(
	struct USBBase *usbbase,
	struct RealFunctionNode *fn,
	struct RealRequest *ioreq,
	U32 CfgNr,
	STR file UNUSED )

#else

SEC_CODE S32 __Config_Set(
	struct USBBase *usbbase,
	struct RealFunctionNode *fn,
	struct RealRequest *ioreq,
	U32 CfgNr )

#endif

{
struct USB3_ConfigNode *cn;
struct USB3_SetupData sd;
S32 retval;
//U8 data[1];

	retval = FALSE;

	#if 0
	{
		struct USB3_EndPointNode **array = fn->fkt_HCD->hn_EndPoint_Array;
		U32 adr = fn->fkt_Address;
		U32 cnt;

		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "\n## Active Config EndPoints (Before)\nIn : " );
		for( cnt=0 ; cnt<16 ; cnt++ )
		{
			usbbase->usb_IExec->DebugPrintF( "%08lx ", array[ adr * 32 + cnt ] );
		}
		usbbase->usb_IExec->DebugPrintF( "\nOut: " );
		for( ; cnt<32 ; cnt++ )
		{
			usbbase->usb_IExec->DebugPrintF( "%08lx ", array[ adr * 32 + cnt ] );
		}
		usbbase->usb_IExec->DebugPrintF( "\n##\n\n" );
		usbbase->usb_IExec->Enable();
	}
	#endif

	// -- Early Check
	if (( fn->fkt_Detach ) || ( fn->fkt_FreeMe ))
	{
		USBDEBUG( "__Config_Set             : Error Funtion Detached" );
		goto bailout;
	}

	// Find Config Nr
	cn = fn->fkt_Configs.uh_Head;

	while(cn)
	{
		if ( cn->cfg_Number == CfgNr )
		{
			break;
		}
		else
		{
			cn = Node_Next( cn );
		}
	}

	if ( CONFIG_VALID(cn) != VSTAT_Okay )
	{
		USBDEBUG( "__Config_Set             : Config not found" );
		goto bailout;
	}

	// Allready Active??
	if ( fn->fkt_Config_Active == cn )
	{
		retval = TRUE;
		USBDEBUG( "__Config_Set             : Config allready active" );
		goto bailout;
	}

	// --

	USBDEBUG( "__Config_Set             : Switching to Config nr #%lu", CfgNr );

	sd.RequestType	= REQTYPE_Write | REQTYPE_Standard | REQTYPE_Device;
	sd.RequestCode	= REQCODE_Set_Configuration;
	sd.Value		= LE_SWAP16( CfgNr );
	sd.Index		= LE_SWAP16( 0 );
	sd.Length		= LE_SWAP16( 0 );

	ioreq->req_Public.io_Command		= CMD_WRITE;
	ioreq->req_Public.io_Data		= NULL;
	ioreq->req_Public.io_Length		= 0;
	ioreq->req_Public.io_SetupData	= & sd;
	ioreq->req_Public.io_SetupLength	= sizeof( struct USB3_SetupData );

	IO_DO( ioreq );

	if ( ioreq->req_Public.io_Error != USB3Err_NoError )
	{
		USBDEBUG( "__Config_Set             : Set Configuration return %ld", (S32) ioreq->req_Public.io_Error );
		goto bailout;
	}

	// -- Settle Time (yikes busy wait)
	MISC_MICRODELAY( 25 * 1000 );	// 25ms

	// --
	#if 0
	// My Competition Pro joystick do not return correct ConfigNr value
	// so disabling for now.. there are proberly many more that would fail

	USBDEBUG( "__Config_Set             : Try and Verifying Config nr" );

	sd.RequestType	= REQTYPE_Read | REQTYPE_Standard | REQTYPE_Device;
	sd.RequestCode	= REQCODE_Get_Configuration;
	sd.Value		= LE_SWAP16( 0 );
	sd.Index		= LE_SWAP16( 0 );
	sd.Length		= LE_SWAP16( 1 );

	ioreq->req_Public.io_Command		= CMD_READ;
	ioreq->req_Public.io_Data			= data;
	ioreq->req_Public.io_Length			= 1;
	ioreq->req_Public.io_SetupData		= & sd;
	ioreq->req_Public.io_SetupLength	= sizeof( struct USB3_SetupData );

	#ifdef DO_PANIC
	data[0] = 172;
	#endif

	IO_DO( ioreq );

	USBDEBUG( "io_Actual ... : %lu", ioreq->req_Public.io_Actual );
	USBDEBUG( "io_Error .... : %ld", (S32) ioreq->req_Public.io_Error );
	USBDEBUG( "data[0] ..... : %lu", (U32) data[0] );

	/**/ if ( ioreq->req_Public.io_Error == USB3Err_Host_Stall )
	{
		// Not all Devices support Get Config
		data[0] = CfgNr;
	}
	else if ( ioreq->req_Public.io_Error != USB3Err_NoError )
	{
		USBDEBUG( "__Config_Set             : Get Configuration return %ld", ioreq->req_Public.io_Error );
		goto bailout;
	}

	if ( data[0] != CfgNr )
	{
		USBDEBUG( "__Config_Set             : Switching Configuration failed ( %lu != %lu )", (U32) data[0], CfgNr );
		goto bailout;
	}
	#endif

	// must not fail
	__Update( usbbase, fn, cn );

	retval = TRUE;

bailout:

	#if 0
	{
		struct USB3_EndPointNode **array = fn->fkt_HCD->hn_EndPoint_Array;
		U32 adr = fn->fkt_Address;
		U32 cnt;

		usbbase->usb_IExec->Disable();
		usbbase->usb_IExec->DebugPrintF( "\n## Active Config EndPoints (After)\nIn : " );
		for( cnt=0 ; cnt<16 ; cnt++ )
		{
			usbbase->usb_IExec->DebugPrintF( "%08lx ", array[ adr * 32 + cnt ] );
		}
		usbbase->usb_IExec->DebugPrintF( "\nOut: " );
		for( ; cnt<32 ; cnt++ )
		{
			usbbase->usb_IExec->DebugPrintF( "%08lx ", array[ adr * 32 + cnt ] );
		}
		usbbase->usb_IExec->DebugPrintF( "\n##\n\n" );
		usbbase->usb_IExec->Enable();
	}
	#endif

	return( retval );
}

// --
