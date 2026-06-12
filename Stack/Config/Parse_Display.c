
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
//#ifdef DO_INFO

SEC_CODE static void __Display( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
struct USB3_InterfaceHeader *ih;
struct USB3_InterfaceGroup *ig;
struct USB3_InterfaceNode *in;
struct USB3_EndPointNode *ep;
struct USB3_ConfigNode *cn;
struct ExecIFace *IExec;

	// --

	IExec = usbbase->usb_IExec;
	IExec->Disable();

	IExec->DebugPrintF( 
		"\n"
		"Vendor: $%04lx, "
		"Device: $%04lx, "
		"Device: $%04lx, "
		"Addr: %ld, "
		"Tier: %ld"
		"\n"
		"FN : %p, "
		"Class: $%04lx, "
		"SubClass: $%04lx, "
		"Protocol: $%04lx"
		"\n",
		LE_SWAP16( fn->fkt_DeviceDescriptor->VendorID ),
		LE_SWAP16( fn->fkt_DeviceDescriptor->DeviceID ),
		LE_SWAP16( fn->fkt_DeviceDescriptor->DeviceVers ),
		fn->fkt_Address, 
		fn->fkt_Tier,
		fn, 
		fn->fkt_Class, 
		fn->fkt_SubClass, 
		fn->fkt_Protocol
	);

	cn = fn->fkt_Configs.uh_Head;

	while(cn)
	{
		IExec->DebugPrintF( "CN : %p, ConfigNr %ld\n", cn, cn->cfg_Number );

		ig = cn->cfg_InterfaceGroups.uh_Head;

		while( ig )
		{
			IExec->DebugPrintF( " IG : %p, Class: $%04lx, SubClass: $%04lx, Protocol: $%04lx\n", ig, ig->ig_Class, ig->ig_SubClass, ig->ig_Protocol );

			ih = ig->ig_Group.uh_Head;

			while( ih )
			{
				IExec->DebugPrintF( "  IH : %p, InterfaceNr: %ld\n", ih, ih->ih_Number );

				in = ih->ih_AltSettings.uh_Head;

				while( in )
				{
					IExec->DebugPrintF( "   IN : %p, AltNr: %ld\n", in, in->in_AltNumber );

					ep = in->in_EndPoints.uh_Head;

					while( ep )
					{
						STR typename;

						/**/ if ( ep->ep_Type == EPATT_Type_Control )
						{
							/**/ if ( ep->ep_Direction == EPDIR_Out )	{ typename = "Control [Out]"; }
							else if ( ep->ep_Direction == EPDIR_In )	{ typename = "Control [In]"; }
							else										{ typename = "Control [??]"; }
						}
						else if ( ep->ep_Type == EPATT_Type_Interrupt )
						{
							/**/ if ( ep->ep_Direction == EPDIR_Out )	{ typename = "Interrupt [Out]"; }
							else if ( ep->ep_Direction == EPDIR_In )	{ typename = "Interrupt [In]"; }
							else										{ typename = "Interrupt [??]"; }
						}
						else if ( ep->ep_Type == EPATT_Type_Bulk )
						{
							/**/ if ( ep->ep_Direction == EPDIR_Out )	{ typename = "Bulk [Out]"; }
							else if ( ep->ep_Direction == EPDIR_In )	{ typename = "Bulk [In]"; }
							else										{ typename = "Bulk [??]"; }
						}
						else if ( ep->ep_Type == EPATT_Type_Isochronous )
						{
							/**/ if ( ep->ep_Direction == EPDIR_Out )	{ typename = "Isochronous [Out]"; }
							else if ( ep->ep_Direction == EPDIR_In )	{ typename = "Isochronous [In]"; }
							else										{ typename = "Isochronous [??]"; }
						}
						else
						{
							/**/ if ( ep->ep_Direction == EPDIR_Out )	{ typename = "?? [Out]"; }
							else if ( ep->ep_Direction == EPDIR_In )	{ typename = "?? [In]"; }
							else										{ typename = "?? [??]"; }
						}

						IExec->DebugPrintF( "    EP : %p, Type: %ld, Num: %ld, %s\n", ep, ep->ep_Type, ep->ep_Number, typename );

						ep = Node_Next( ep );
					}

					in = Node_Next( in );
				}

				ih = Node_Next( ih );
			}

			ig = Node_Next( ig );
		}

		cn = Node_Next( cn );
	}

	IExec->DebugPrintF( "\n" );

	IExec->Enable();
}

//#endif
// --
