
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE struct USB3_FktDriverNode *__FDriver_CreateList( struct USBBase *usbbase, struct TagItem *taglist )
{
struct USB3_FktDriverNode *fdn;
struct TagItem *tag;
int error;

	USBDEBUG( "__FDriver_CreateList" );

	error = TRUE;

	// --

	fdn = FDRIVER_ALLOC();

	if ( ! fdn )
	{
		USBERROR( "__FDriver_CreateList : Error allocating node" );
		goto bailout;
	}

	// Default SubClass/Protocol to wildcard if not specified
	fdn->fdn_SubClass = FDSUBCLASS_Any;
	fdn->fdn_Protocol = FDPROTOCOL_Any;

	// --

	while(( tag = MISC_NEXTTAGITEM( & taglist )))
	{
		switch ( tag->ti_Tag )
		{
			case USB3Tag_FDriver_Title:
			{
//				USBDEBUG( "FDriver_Title .......... : %s", tag->ti_Data );
				USBERROR( "FDriver_Title .......... : %s", tag->ti_Data );

				if ( fdn->fdn_Title )
				{
					MEM_FREEVEC( fdn->fdn_Title );
				}

				// Copy String
				fdn->fdn_Title = MEM_PRINTF( "%s", tag->ti_Data );
				break;
			}

			case USB3Tag_FDriver_Type:
			{
//				USBDEBUG( "FDriver_Type ........... : %ld", tag->ti_Data );
				USBERROR( "FDriver_Type ........... : %ld", tag->ti_Data );

				if ( tag->ti_Data < 3 )
				{
					fdn->fdn_Type = tag->ti_Data;
				}
				else
				{
					USBERROR( "__FDriver_CreateList : Invalid TagData (%ld) for Type", tag->ti_Data );
				}
				break;
			}

			case USB3Tag_FDriver_Driver_Filename:
			{
//				USBDEBUG( "FDriver_FileName ....... : %s", tag->ti_Data );
				USBERROR( "FDriver_FileName ....... : %s", tag->ti_Data );

				if ( fdn->fdn_Driver_Filename )
				{
					MEM_FREEVEC( fdn->fdn_Driver_Filename );
				}

				// Copy String
				fdn->fdn_Driver_Filename = MEM_PRINTF( "%s", tag->ti_Data );
				break;
			}

			case USB3Tag_FDriver_Class_Filename:
			{
//				USBDEBUG( "FDriver_ClassName ...... : %s", tag->ti_Data );
				USBERROR( "FDriver_ClassName ...... : %s", tag->ti_Data );

				if ( fdn->fdn_Class_Filename )
				{
					MEM_FREEVEC( fdn->fdn_Class_Filename );
				}

				// Copy String
				fdn->fdn_Class_Filename = MEM_PRINTF( "%s", tag->ti_Data );
				fdn->fdn_Class_FilenameLen = MISC_STRLENGTH( fdn->fdn_Class_Filename );
				break;
			}

			case USB3Tag_FDriver_Priority:
			{
//				USBDEBUG( "FDriver_Priority ....... : %ld", tag->ti_Data );
				USBERROR( "FDriver_Priority ....... : %ld", tag->ti_Data );

				fdn->fdn_Pri = tag->ti_Data;
				break;
			}

			case USB3Tag_FDriver_Class:
			{
//				USBDEBUG( "FDriver_Class .......... : %ld", tag->ti_Data );
				USBERROR( "FDriver_Class .......... : %ld", tag->ti_Data );

				if (( tag->ti_Data < 256 ) || ( tag->ti_Data == FDCLASS_Any ))
				{
					fdn->fdn_Class = tag->ti_Data;
				}
				else
				{
					USBERROR( "__FDriver_CreateList : Invalid TagData (%08lx) for Class", tag->ti_Data );
				}
				break;
			}

			case USB3Tag_FDriver_SubClass:
			{
//				USBDEBUG( "FDriver_SubClass ....... : %04lx", tag->ti_Data );
				USBERROR( "FDriver_SubClass ....... : %04lx", tag->ti_Data );

				if (( tag->ti_Data < 256 ) || ( tag->ti_Data == FDSUBCLASS_Any ))
				{
					fdn->fdn_SubClass = tag->ti_Data;
				}
				else
				{
					USBERROR( "__FDriver_CreateList : Invalid TagData (%08lx) for SubClass", tag->ti_Data );
				}
				break;
			}

			case USB3Tag_FDriver_Protocol:
			{
//				USBDEBUG( "FDriver_Protocol ....... : %04lx", tag->ti_Data );
				USBERROR( "FDriver_Protocol ....... : %04lx", tag->ti_Data );

				if (( tag->ti_Data < 256 ) || ( tag->ti_Data == FDPROTOCOL_Any ))
				{
					fdn->fdn_Protocol = tag->ti_Data;
				}
				else
				{
					USBERROR( "__FDriver_CreateList : Invalid TagData (%08lx) for Protocol", tag->ti_Data );
				}
				break;
			}

			case USB3Tag_FDriver_Entry:
			{
//				USBDEBUG( "FDriver_DriverEntry .... : %p", tag->ti_Data );
				USBERROR( "FDriver_DriverEntry .... : %p", tag->ti_Data );

				fdn->fdn_Entry = (PTR) tag->ti_Data;
				break;
			}

			// --

			default:
			{
//				USBDEBUG( "__FDriver_CreateList : Unknown tag [%08lx:%08lx]", tag->ti_Tag, tag->ti_Data );
				USBERROR( "__FDriver_CreateList : Unknown tag [%08lx:%08lx]", tag->ti_Tag, tag->ti_Data );
				break;
			}
		}
	}

	// --

//	usbbase->usb_IExec->DebugPrintF( "\nAdding FDriver Class %ld\n\n", fdn->fdn_Class );

	SEMAPHORE_OBTAIN(	& usbbase->usb_FDriver_Semaphore );
	NODE_ADDTAIL(		& usbbase->usb_FDriver_Header, fdn );
	SEMAPHORE_RELEASE(	& usbbase->usb_FDriver_Semaphore );

	// --

	error = FALSE;

bailout:

	if (( fdn ) && ( error ))
	{
		FDRIVER_FREE( fdn );
		fdn = NULL;
	}

	return( fdn );
}

// --

SEC_CODE struct USB3_FktDriverNode * VARARGS68K __FDriver_CreateTags( struct USBBase *usbbase, ... )
{
struct USB3_FktDriverNode *fdn;
va_list ap;

	USBDEBUG( "__FDriver_CreateTags" );

	va_start( ap, usbbase );

	fdn = FDRIVER_CREATELIST( va_getlinearva( ap, struct TagItem * ));

	va_end( ap );

	return( fdn );
}

// --
