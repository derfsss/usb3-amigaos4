
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "MSD.h"

// --
#if 1

SEC_RODATA const PTR MSD_Manager_Vectors[] =
{
	(PTR) MSD_Manager_Obtain,
	(PTR) MSD_Manager_Release,
	(PTR) NULL,
	(PTR) NULL,
	(PTR) MSD_Manager_Open,
	(PTR) MSD_Manager_Close,
	(PTR) MSD_Manager_Expunge,
	(PTR) NULL,
	(PTR) MSD_Manager_BeginIO,
	(PTR) MSD_Manager_AbortIO,
	(PTR) -1
};

SEC_RODATA static const struct TagItem MSD_Manager_Tags[] =
{
	{ MIT_Name,			(Tag) "__device"		},
	{ MIT_VectorTable,	(Tag) MSD_Manager_Vectors	},
	{ MIT_Version,		(Tag) 1					},
	{ TAG_END,			(Tag) 0					}
};

SEC_RODATA static const PTR devInterfaces[] =
{
	(PTR) MSD_Manager_Tags,
	(PTR) NULL
};

SEC_RODATA static const struct TagItem devCreateTags[] =
{
	{ CLT_DataSize,		(Tag) sizeof( struct MSDBase ) },
	{ CLT_InitFunc,		(Tag) MSD_Manager_Init		},
	{ CLT_Interfaces,	(Tag) devInterfaces		},
	{ TAG_END,			(Tag) 0					}
};

#endif
// --

SEC_CODE S32 MSD_CreateDevice( struct USBBase *usbbase )
{
struct MSDBase *MSDBase;
S32 retval;

	TASK_NAME_ENTER( "MSD_CreateDevice" );

	retval = FALSE;

	if ( ! usbbase->usb_MSDDevice )
	{
		MSDBase = MISC_CREATELIBRARY( (PTR) devCreateTags );

		if ( MSDBase )
		{
			usbbase->usb_MSDDevice = (PTR) MSDBase;

			MSDBase->msdbase_usbbase = usbbase;

			SEMAPHORE_INIT( & MSDBase->msdbase_MSDDisk_Semaphore );

			MISC_ADDDEVICE( MSDBase );
		}
	}

	if ( ! usbbase->usb_MSDDevice )
	{
		USBERROR( "Error creating MSD Device" );
		goto bailout;
	}

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();
	return(	retval );
}

// --
