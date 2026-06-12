
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
// Safe String Compare, can handle NULL Pointers
// case-sensitive string comparison

#if 0

// todo

SEC_CODE S32 __Misc_StrCaseCompare( struct USBBase *usbbase, STR str1, STR str2 )
{
S32 retval;

	TASK_NAME_ENTER( "__Misc_StrCaseCompare" );

	if (( ! str1 ) || ( ! str2 ))
	{
		retval = -1;
	}
	else
	{
		retval = usbbase->usb_IUtility->Stricmp( str1, str2 );
	}

	TASK_NAME_LEAVE();

	return( retval );
}

#endif
// --
