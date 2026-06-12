
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

SEC_CODE STR __Misc_LoadFile( struct USBBase *usbbase, STR filename, U32 *filesize )
{
S32 handle;
U32 size;
STR mem;
S32 err;

	TASK_NAME_ENTER( "__DOS_LoadFile" );

	mem = NULL;

	err = TRUE;

	size = 0;

	handle = MISC_OPENFILE( filename, MODE_OLDFILE );

	if ( ! handle )
	{
		USBERROR( "File not found" );
		goto bailout;
	}

	size = MISC_GETFILESIZE( handle );

	if ( ! size )
	{
		USBERROR( "Error getting file size" );
		goto bailout;
	}

	mem = MEM_ALLOCVEC( size+1, FALSE );

	if ( ! mem )
	{
		USBERROR( "Error allocating memory" );
		goto bailout;
	}

	if ( MISC_READFILE( handle, mem, size ) != (S32) size )
	{
		USBERROR( "Error reading file" );
		goto bailout;
	}

	// Now the text is NUL terminated
	mem[size] = 0;

	err = FALSE;

bailout:

	if ( handle )
	{
		MISC_CLOSEFILE( handle );
		handle = 0;
	}

	if (( err ) && ( mem ))
	{
		MEM_FREEVEC( mem );
		mem = NULL;
	}

	if ( filesize )
	{
		*filesize = ( err ) ? 0 : size ;
	}

	TASK_NAME_LEAVE();

	return( mem );
}

// --
