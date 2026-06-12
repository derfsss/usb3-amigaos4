
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

SEC_CODE enum VSTAT __Interface_ValidNode( struct USBBase *usbbase UNUSED, struct USB3_InterfaceNode *in, STR file )

#else

SEC_CODE enum VSTAT __Interface_ValidNode( struct USBBase *usbbase UNUSED, struct USB3_InterfaceNode *in )

#endif

{
enum VSTAT vstat;

	TASK_NAME_ENTER( "__Interface_ValidNode" );

	/**/ if ( ! in )
	{
		vstat = VSTAT_Null;
	}

	#ifdef DO_PANIC

	else if ( in == (PTR) 0x55555555 )
	{
		USBPANIC( "__Interface_ValidNode    : IN    %p : Free'd Memory : (%s)", in, (file)?file:"<NULL>" );
		vstat = VSTAT_Null;
	}

	#endif

	else if ( in->in_StructID == 0 )
	{
		USBDEBUG( "__Interface_ValidNode    : IN    %p : Not Initalized", in );
		vstat = VSTAT_Null;
	} 
	else if ( in->in_StructID == ID_USB3_FREED )
	{
		USBDEBUG( "__Interface_ValidNode    : IN    %p : Free Struct", in );
		vstat = VSTAT_Null;
	}
	else if ( in->in_StructID != ID_USB3_IFCN )
	{
		USBDEBUG( "__Interface_ValidNode    : IN    %p : Invalid ID ($%08lx)", in, in->in_StructID );
		vstat = VSTAT_Error;
	}
	else
	{
		vstat = VSTAT_Okay;
	}

//	USBDEBUG( "__Interface_ValidNode    : IN    %p : Stat %ld", in, vstat );

	TASK_NAME_LEAVE();

	return( vstat );
}

// --
