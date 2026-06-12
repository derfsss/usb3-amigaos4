
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE struct USB2_Descriptor *__Desc_Next_Desc( struct USBBase *usbbase UNUSED, struct USB2_Descriptor *desc )
{
struct USB2_Descriptor *dsc;
U32 pos;
U8 *data;

	dsc = NULL;

	if (( desc ) 
	&&	( desc->Length ) 
	&&	( desc->DescriptorType ))
	{
		pos		= desc->Length;
		data	= (PTR) desc;
		desc	= (PTR) & data[pos];

		if (( desc->Length ) 
		&&	( desc->DescriptorType ))
		{
			dsc = desc;
		}
	}

	return( dsc );
}

// --
