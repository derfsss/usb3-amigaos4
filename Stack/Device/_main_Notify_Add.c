
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

static void Send_HCDs( struct USBBase *usbbase, struct USB3_NotifyNode *node );
static void Send_Functions( struct USBBase *usbbase, struct USB3_NotifyNode *node );
static void Send_Interfaces( struct USBBase *usbbase, struct USB3_NotifyNode *node );

// --

SEC_CODE static PTR _main_Notify_Add( struct USB3IFace *Self, U32 type, struct MsgPort *mp )
{
struct USB3_NotifyNode *node;
struct USBBase *usbbase;

	usbbase = (PTR) Self->Data.LibBase;

	node = NULL;

	if ( type >= USBNT_Last )
	{
		USBERROR( "Error unsuported Notify type (%ld)", type );
		goto bailout;
	}

	if ( ! mp )
	{
		USBERROR( "Error MsgPort pointer is NULL" );
		goto bailout;
	}

	node = MEMORY_ALLOC( MEMID_NotifyNode, TRUE );

	if ( ! node )
	{
		USBERROR( "Error allocating memory" );
		goto bailout;
	}

	node->nn_StructID = ID_USB3_NN;
	node->nn_MsgPort = mp;
	node->nn_Type = type;

	// When we add a notify, we send out a list of all added nodes

	switch( node->nn_Type )
	{
		case USBNT_HCD:			Send_HCDs(			usbbase, node );	break;
		case USBNT_Function:	Send_Functions( 	usbbase, node );	break;
		case USBNT_Interface:	Send_Interfaces(	usbbase, node );	break;
		default:														break;
	}

	SEMAPHORE_OBTAIN(	& usbbase->usb_Notify_Semaphore );
	NODE_ADDTAIL(		& usbbase->usb_Notify_Header, node );
	SEMAPHORE_RELEASE(	& usbbase->usb_Notify_Semaphore );

bailout:

	return(	node );
}

// --

static void Send_HCDs( struct USBBase *usbbase, struct USB3_NotifyNode *node )
{
struct USB3_NotifyMessage *msg;
struct USB3_HCDNode *hcd;
struct ExecIFace *IExec;

	// HCD is a Static list build at Startup
	// so no locking is nesary.

	hcd = usbbase->usb_HCDHeader.uh_Head;

	while( hcd )
	{
		if ( hcd->hn_Task )
		{
			msg = MEMORY_ALLOC( MEMID_NotifyMessage, TRUE );

			if ( ! msg )
			{
				USBERROR( "Error allocating memory" );
				break;
			}

			// Default Notify Data

			msg->nm_Message.mn_ReplyPort= & usbbase->usb_Notify_ReplyMsgPort->ump_MsgPort;
			msg->nm_Message.mn_Length	= sizeof( struct USB3_NotifyMessage );
			msg->nm_Type				= USBNT_HCD;
			msg->nm_Command 			= USBNC_Adding;
			msg->nm_ID					= hcd->hn_NotifyID;

			// Function Specific Info

			msg->nm_Data.nm_HCD.nm_Type	= hcd->hn_HCDType;
			msg->nm_Data.nm_HCD.nm_Index= hcd->hn_HCDIndex;

			IExec = usbbase->usb_IExec;
			IExec->PutMsg( node->nn_MsgPort, & msg->nm_Message );
		}

		hcd = Node_Next( hcd );
	}

}

// --

static void Send_Functions( struct USBBase *usbbase, struct USB3_NotifyNode *node )
{
struct USB3_NotifyMessage *msg;
struct RealFunctionNode *fn;
struct ExecIFace *IExec;
struct USB3_Node *n;
U32 hcdid;

	SEMAPHORE_OBTAIN( & usbbase->usb_Fkt_Semaphore );

	fn = NULL;

	n = usbbase->usb_Fkt_Header.uh_Tail;

	while( n )
	{
		fn = n->un_Data;

		if (( fn ) && ( ! fn->fkt_Detach ))
		{
			msg = MEMORY_ALLOC( MEMID_NotifyMessage, TRUE );

			if ( ! msg )
			{
				USBERROR( "Error allocating memory" );
				break;
			}

			// Default Notify Data

			msg->nm_Message.mn_ReplyPort= & usbbase->usb_Notify_ReplyMsgPort->ump_MsgPort;
			msg->nm_Message.mn_Length	= sizeof( struct USB3_NotifyMessage );
			msg->nm_Type				= USBNT_Function;
			msg->nm_Command 			= USBNC_Adding;
			msg->nm_ID					= fn->fkt_NotifyID;

			// Function Specific Info

			hcdid = fn->fkt_HCD->hn_NotifyID;

			msg->nm_Data.nm_Function.nm_HCD_ID		= hcdid;
			msg->nm_Data.nm_Function.nm_Parent_ID	= ( fn->fkt_Parent ) ? fn->fkt_Parent->fkt_NotifyID : hcdid;
			msg->nm_Data.nm_Function.nm_PortNr		= fn->fkt_PortNr;

			IExec = usbbase->usb_IExec;
			IExec->PutMsg( node->nn_MsgPort, & msg->nm_Message );
		}

		n = n->un_Prev;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Fkt_Semaphore );

}

// --

static void Send_Interfaces( struct USBBase *usbbase, struct USB3_NotifyNode *node )
{
struct USB3_InterfaceHeader *ih;
struct USB3_InterfaceGroup *ig;
struct USB3_InterfaceNode *in;
struct USB3_NotifyMessage *msg;
struct RealFunctionNode *fn;
struct USB3_ConfigNode *cn;
struct ExecIFace *IExec;
struct USB3_Node *n;

	SEMAPHORE_OBTAIN( & usbbase->usb_Fkt_Semaphore );

	fn = NULL;

	n = usbbase->usb_Fkt_Header.uh_Tail;

	while( n )
	{
		fn = n->un_Data;

		if (( fn ) && ( ! fn->fkt_Detach ))
		{
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

						msg = MEMORY_ALLOC( MEMID_NotifyMessage, TRUE );

						if ( ! msg )
						{
							USBERROR( "Error allocating memory" );
							break;
						}

						// Default Notify Data

						msg->nm_Message.mn_ReplyPort= & usbbase->usb_Notify_ReplyMsgPort->ump_MsgPort;
						msg->nm_Message.mn_Length	= sizeof( struct USB3_NotifyMessage );
						msg->nm_Type				= USBNT_Interface;
						msg->nm_Command 			= USBNC_Adding;
						msg->nm_ID					= ih->ih_NotifyID;

						// Function Specific Info

						msg->nm_Data.nm_Interface.nm_Config_Nr			= cn->cfg_Number;
						msg->nm_Data.nm_Interface.nm_Config_StringID	= cn->cfg_StringID;
						msg->nm_Data.nm_Interface.nm_Function_ID		= fn->fkt_NotifyID;
						msg->nm_Data.nm_Interface.nm_Interface_Nr  		= ih->ih_Number;
						msg->nm_Data.nm_Interface.nm_Interface_AltNr	= ih->ih_AltNumber;
						msg->nm_Data.nm_Interface.nm_Interface_StringID	= in->in_StringID;

						IExec = usbbase->usb_IExec;
						IExec->PutMsg( node->nn_MsgPort, & msg->nm_Message );

						ih = Node_Next( ih );
					}

					ig = Node_Next( ig );
				}
			}
		}

		n = n->un_Prev;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_Fkt_Semaphore );
}

// --
