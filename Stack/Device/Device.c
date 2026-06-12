
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "usb3_rev.h"

// --

#include "Device_Shutdown.c"
#include "Device_Startup.c"

// --

#include "_man_Obtain.c"
#include "_man_Release.c"
#include "_man_Expunge.c"
#include "_man_Open.c"
#include "_man_Close.c"
#include "_man_BeginIO.c"
#include "_man_AbortIO.c"

// --

#include "_main_Obtain.c"
#include "_main_Release.c"

#include "_main_Attr_Get.c"
#include "_main_Attr_Set.c"

#include "_main_Fkt_Find.c"

#include "_main_Ifc_Find.c"
#include "_main_Ifc_Claim.c"
#include "_main_Ifc_Declaim.c"

#include "_main_Reg_Register.c"
#include "_main_Reg_Unregister.c"

#include "_main_EPRes_Obtain.c"
#include "_main_EPRes_Release.c"
#include "_main_EPRes_Destall.c"

#include "_main_Notify_Add.c"
#include "_main_Notify_Remove.c"

// --

SEC_RODATA static const PTR _manager_Vectors[] =
{
	(PTR) _manager_Obtain,
	(PTR) _manager_Release,
	(PTR) NULL,
	(PTR) NULL,
	(PTR) _manager_Open,
	(PTR) _manager_Close,
	(PTR) _manager_Expunge,
	(PTR) NULL,
	(PTR) _manager_BeginIO,
	(PTR) _manager_AbortIO,
	(PTR) -1
};

SEC_RODATA static const struct TagItem _manager_Tags[] =
{
	{ MIT_Name,			(Tag) "__device"		},
	{ MIT_VectorTable,	(Tag) _manager_Vectors	},
	{ MIT_Version,		(Tag) 1					},
	{ TAG_END,			(Tag) 0					}
};

// --

SEC_RODATA static const PTR main_v1_vectors[] =
{
	(PTR) _main_Obtain,
	(PTR) _main_Release,
	(PTR) NULL,
	(PTR) NULL,

	(PTR) _main_Attr_Get,
	(PTR) _main_Attr_Set,

	(PTR) _main_Fkt_FindTags,
	(PTR) _main_Fkt_FindList,

	(PTR) _main_Ifc_FindTags,
	(PTR) _main_Ifc_FindList,
	(PTR) _main_Ifc_Claim,
	(PTR) _main_Ifc_Declaim,

	(PTR) _main_Reg_RegisterTags,
	(PTR) _main_Reg_RegisterList,
	(PTR) _main_Reg_Unregister,

	(PTR) _main_EPRes_ObtainTags,
	(PTR) _main_EPRes_ObtainList,
	(PTR) _main_EPRes_Release,
	(PTR) _main_EPRes_Destall,

	(PTR) _main_Notify_Add,
	(PTR) _main_Notify_Remove,

	(PTR) -1
};

SEC_RODATA static const struct TagItem _main_Tags[] =
{
	{ MIT_Name,			(Tag) "main"			},
	{ MIT_VectorTable,	(Tag) main_v1_vectors	},
	{ MIT_Version,		(Tag) 1					},
	{ TAG_END,			(Tag) 0					}
};

// --

SEC_RODATA const PTR devInterfaces[] =
{
	(PTR) _manager_Tags,
	(PTR) _main_Tags,
	(PTR) NULL
};

// --
// Note: don't add Static

SEC_RODATA const char verstag[] = VERSTAG;

// --
// if we set "usb3.device" it can we loaded from AmigaDOS
// for kickstart module, its just an info string

SEC_RWDATA const struct Resident usb_res =
{
/* rt_MatchWord	*/			RTC_MATCHWORD,
/* rt_MatchTag	*/ (PTR)	( & usb_res + 0 ),
/* rt_EndSkip	*/ (PTR)	( & usb_res + 1 ),
/* rt_Flags		*/			RTF_NATIVE|RTF_COLDSTART,
/* rt_Version	*/			VERSION,
/* rt_Type		*/			NT_UNKNOWN,
/* rt_Pri		*/			1,
/* rt_Name		*/ (STR)	"usb3.device", //"Universal Serial Bus",
/* rt_IdString	*/ (STR)	VSTRING,
/* rt_Init		*/ (PTR)	ROMInit
};

// --

SEC_CODE S32 _start( STR argstring UNUSED, S32 arglen UNUSED, struct ExecBase *sysbase UNUSED )
{
	return( 0 );
}

// --
