
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE static S32 __myAnyStrings( struct USBBase *usbbase, struct RealFunctionNode *fn )
{
struct USB2_Descriptor *dsc;
S32 retval;

	retval = TRUE;

	if ( fn->fkt_DeviceDescriptor->ManufacturerStrNr )
	{
		goto found;
	}

	if ( fn->fkt_DeviceDescriptor->DeviceStrNr )
	{
		goto found;
	}

	if ( fn->fkt_DeviceDescriptor->SerialStrNr )
	{
		goto found;
	}

	dsc = fn->fkt_Config_Desc_Buf;

	while( dsc )
	{
		switch( dsc->DescriptorType )
		{
			case DSCTYPE_Configuration:
			{
				if ( ((struct USB2_Config_Desc *)dsc)->ConfigurationStrNr )
				{
					goto found;
				}
				break;
			}

			case DSCTYPE_Interface:
			{
				if ( ((struct USB2_Interface_Desc *)dsc)->InterfaceStrNr )
				{
					goto found;
				}
			}
		}

		dsc = DESC_NEXT_DESC( dsc );
	}

	retval = FALSE;

found:

	return( retval );
}

// --

#define __GETSTRING(x,y,z)		__myGetString(usbbase,fn,ioreq,sd,x,y,z)

static S32 __myGetString(
	struct USBBase *usbbase,
	struct RealFunctionNode *fn,
	struct USB2_IORequest *ioreq,
	struct USB2_SetupData *sd,
	U32 id,
	U16 **StringBuf,
	S32 *StringLen )
{
S32 retval;
STR buf;

	retval = FALSE;

	buf = MEM_ALLOCIOBUFFER( 256, FALSE );

	if ( ! buf )
	{
		USBERROR( "__myGetString: Error allocating memory" );
		goto bailout;
	}

	sd->RequestType	= REQTYPE_Read | REQTYPE_Standard | REQTYPE_Device;
	sd->RequestCode	= REQCODE_Get_Descriptor;
	sd->Value		= LE_SWAP16( (( DSCTYPE_String << 8 ) | ( id )) );
	sd->Index		= LE_SWAP16( fn->fkt_LanguageID );
	sd->Length		= LE_SWAP16( 255 );

	ioreq->io_Data	= buf;
	ioreq->io_Length= 255;

	IO_DO(ioreq);

	USBDEBUG( "io_Error  %ld", ioreq->io_Error );
	USBDEBUG( "io_Actual %ld", ioreq->io_Actual );

	if ( ioreq->io_Error )
	{
		USBERROR( "__myGetString: io_Error %ld", ioreq->io_Error );
		goto bailout;
	}

	if ( ioreq->io_Actual < 2 )
	{
		USBERROR( "__myGetString: io_Actual (%ld < 2)", ioreq->io_Actual );
		goto bailout;
	}

	if ( buf[1] != DSCTYPE_String )
	{
		USBERROR( "__myGetString: Expected a String Descriptor (%ld != %ld)", buf[1], DSCTYPE_String );
		goto bailout;
	}

	*StringLen = buf[0];
	*StringBuf = MEM_ALLOCVEC( buf[0]+1, TRUE );

	if ( ! *StringBuf )
	{
		USBERROR( "__myGetString: Error Allocating Memory" );
		goto bailout;
	}

	MEM_COPY( buf, *StringBuf, MIN( ioreq->io_Actual, buf[0] ));

	retval = TRUE;

bailout:

	if (( buf ) && ( ioreq->io_Error ))
	{
		// todo : Destall
	}

	MEM_FREEIOBUFFER( buf );

	return( retval );
}

// --

SEC_CODE static S32 __Get_Strings( 
	struct RealFunctionNode *fn,
	struct USB2_IORequest *ioreq,
	struct USB2_SetupData *sd,
	struct USBBase *usbbase )
{
struct USB2_InterfaceHeader *ih;
struct USB2_InterfaceGroup *ig;
struct USB2_InterfaceNode *in;
struct USB2_String_Desc *strdsc;
struct USB2_ConfigNode *cn;
U16 *LangBuf;
S32 LangLen;
S32 retval;
S32 len;
S32 cnt;
U32 id;

	retval = FALSE;

	ioreq->io_Command		= CMD_READ;
	ioreq->io_SetupData		= sd;
	ioreq->io_SetupLength	= sizeof( struct USB2_SetupData );

	USBDEBUG( "Checking for Strings" );

	if ( __myAnyStrings( usbbase, fn ))
	{
		USBDEBUG( "Have Strings" );

		// --

		USBDEBUG( "Getting LangID" );
		if ( ! __GETSTRING( 0, & LangBuf, & LangLen ))
		{
			USBERROR( "__myGetString: Error Getting Language (%ld)", (S32) ioreq->io_Error );
			goto bailout;
		}

		// -- Check Lang

		strdsc = (PTR) LangBuf;
		len = ( strdsc->Length - 2 ) / 2;

		if ( strdsc->Length >= 4 )
		{
			// Try and find 0x0409 (US English)

			for( cnt=0 ; cnt<len ; cnt++ )
			{
				if ( strdsc->LangID[cnt] == LE_SWAP16( 0x0409 ))
				{
					break;
				}
			}

			if ( cnt == len )
			{
				// Not found, just use the first LandID returned
				fn->fkt_LanguageID = LE_SWAP16( strdsc->LangID[0] );
			}
			else
			{
				// Found using English United States (Normal Default)
				fn->fkt_LanguageID = 0x0409;
			}
		}

		MEM_FREEVEC( LangBuf );

		if ( ! fn->fkt_LanguageID )
		{
			USBERROR( "__myGetString: Error Obtaining Language Code" );
			goto bailout;
		}

		USBINFO( "Using LanguageID: 0x%04lx", fn->fkt_LanguageID );

		/*
		** Get Manufacturer String
		*/

		id = fn->fkt_DeviceDescriptor->ManufacturerStrNr;
		if ( id )
		{
			USBINFO( "Getting Manufacturer String (%lu)", id );
			if ( ! __GETSTRING( id, & fn->fkt_ManufacturerStr, & fn->fkt_ManufacturerStrLen ))
			{
				USBERROR( "__myGetString: Error Getting Device String (%ld)", (S32) ioreq->io_Error );
//				goto bailout;
			}
		}

		/*
		** Get Device String
		*/

		id = fn->fkt_DeviceDescriptor->DeviceStrNr;
		if ( id )
		{
			USBINFO( "Getting Device String (%lu)", id );
			if ( ! __GETSTRING( id, & fn->fkt_DeviceStr, & fn->fkt_DeviceStrLen ))
			{
				USBERROR( "__myGetString: Error Getting Device String (%ld)", (S32) ioreq->io_Error );
//				goto bailout;
			}
		}

		/*
		** Get Serial String
		*/

		id = fn->fkt_DeviceDescriptor->SerialStrNr;
		if ( id )
		{
			USBINFO( "Getting Serial String (%lu)", id );
			if ( ! __GETSTRING( id, & fn->fkt_SerialStr, & fn->fkt_SerialStrLen ))
			{
				USBERROR( "__myGetString: Error Getting Serial String (%ld)", (S32) ioreq->io_Error );
//				goto bailout;
			}
		}

		/*
		** Check for Config/Interface Strings
		*/

		cn = fn->fkt_Configs.uh_Head;

		while(cn)
		{
			// -- Config

			id = cn->cfg_Descriptor->ConfigurationStrNr;

			if ( id )
			{
				USBINFO( "Get Configuration String (%lu)", id );

				if ( ! __GETSTRING( id, & cn->cfg_String, & cn->cfg_StringLen ))
				{
					USBERROR( "__myGetString: Error Getting Config String (%ld)", (S32) ioreq->io_Error );
//					goto bailout;
				}

				cn->cfg_StringID = MISC_NEWNOTIFYID();
			}

			// -- Interface

			ig = cn->cfg_InterfaceGroups.uh_Head;

			while( ig )
			{
				ih = ig->ig_Group.uh_Head;

				while( ih )
				{
					in = ih->ih_AltSettings.uh_Head;

					while( in )
					{
						id = in->in_Descriptor->InterfaceStrNr;

						if ( id )
						{
							USBINFO( "Get Interface String (%lu)", id );

							if ( ! __GETSTRING( id, & in->in_String, & in->in_StringLen ))
							{
								USBERROR( "__myGetString: Error Getting Interface String (%ld)", (S32) ioreq->io_Error );
//								goto bailout;
							}

							in->in_StringID = MISC_NEWNOTIFYID();
						}

						in = Node_Next(in);
					}

					ih = Node_Next( ih );
				}
			
				ig = Node_Next( ig );
			}

			// -- 

			cn = Node_Next( cn );
		}
	}
	else
	{
		USBINFO( "No Strings Found, skipping" );
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --
