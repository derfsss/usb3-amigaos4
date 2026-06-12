
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
// Small code for validating Return values

SEC_CODE S32 myIS_TASKRETURN_ERR( U32 value )
{
S32 err;

	if ( IS_TASKRETURN( value ))
	{
		// is a Valid Stack ReturnCode

		err = ( value & TASKRETURN_FLAG ) ? TRUE : FALSE ;
	}
	else
	{
		// is a User ReturnCode

		err = ( value ) ? FALSE : TRUE ;
	}

	return( err );
}

// --
// Small code for getting/converting Return values

SEC_CODE U32 myGET_TASKRETURN_CODE( U32 value )
{
U32 code;

	if ( IS_TASKRETURN( value ))
	{
		// is a Valid Stack ReturnCode

		code = value;
	}
	else
	{
		// is a User ReturnCode

		code = ( value ) ? TASK_Return_Stack_Okay : TASK_Return_Stack_Error ;
	}

	return( code );
}

// --
