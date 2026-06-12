
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

S32 Process_Setup_Device( struct EthernetUnit *unit )
{
struct USB3_IORequest *usbio;
S32 retval;
U32 cnt;

	USBERROR( "RTL8188 : __myProcess_Setup_Device" );

	retval = FALSE;

	unit->unit_USB_Register = USB3_Reg_RegisterTags(
		USB3Tag_Reg_Interface, 	unit->unit_USB_Interface,
//		USB3Tag_Reg_DriverMessage, in->StartMsg,
//		USB3Tag_Reg_TimeOut, 1000000*1,		// 1 sec
		USB3Tag_Reg_Title, "RTL8188",
		TAG_END
	);

	if ( ! unit->unit_USB_Register )
	{
		USBERROR( "RTL8188 : Failed to Register Device : Ifc %p", unit->unit_USB_Interface );
		goto bailout;
	}

	unit->unit_Bulk_Tx_Resource = USB3_EPRes_ObtainTags( unit->unit_USB_Register,
		USB3Tag_EPRes_NrOfIORequest, NR_of_TXs,
		USB3Tag_EPRes_EPDirection, EPADR_Dir_Out,
		USB3Tag_EPRes_EPType, EPATT_Type_Bulk,
//		USB3Tag_BufferSize, 1522,
		TAG_END
	);

	if ( ! unit->unit_Bulk_Tx_Resource )
	{
		USBERROR( "RTL8188 : Error obtaining EndPoint Resource" );
		goto bailout;
	}

	for( cnt=0 ; cnt<NR_of_TXs ; cnt++ )
	{
		usbio = unit->unit_Bulk_Tx_Resource->IORequests[cnt];
		usbio->io_UserValue = cnt;

		AddTail( unit->unit_USB_IOReq_FreeList, (PTR) usbio );
	}

	retval = TRUE;

bailout:

	return( retval );
}
