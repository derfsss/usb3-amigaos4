
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

struct CmdStruct
{
	U16	CmdLen;
	STR	CmdName;
	U16	CmdType;
};

enum
{
	_Title,
	_FNClass,	// Class Filename
	_FNDriver,	// Driver Filename
	_Type,
	_Class,
	_SubClass,
	_Protocol,
// _StackSize,
// _Priority,
// _Hotkey,
// _Device,
// _Date,
// // --
// _Device,
// _Quirk,
	// --
	_LAST
};

static struct CmdStruct Cmds[] =
{
	{ 5, "TITLE",		_Title		},
	{ 6, "DRIVER",		_FNDriver	},
	{ 4, "TYPE",		_Type		},
	{ 5, "CLASS",		_Class		},
	{ 8, "SUBCLASS",	_SubClass	},
	{ 8, "PROTOCOL",	_Protocol	},
//	{ 5, "STACK",		_StackSize	},
//	{ 8, "PRIORITY",	_Priority	},
//	{ 6, "DEVICE",		_Device		},	// Device:Device:Quirk
//	{ 6, "HOTKEY",		_Hotkey		},
	{ 0, NULL,			0			},
};

static U32 _decnumbers[] =
{
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000
};

static int8 HexData[256] =
{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 0
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 1
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 2
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,	// 3
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 4
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 5
	-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 6
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 7
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 8
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 9
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// A
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// B
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// C
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// D
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// E
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// F
};

// --

SEC_CODE static U32 _GetDecimal( STR str )
{
U32 retval;
U32 pos;
U32 len;
U32 cnt;

	retval = 0;

	if ( ! str )
	{
		goto bailout;
	}

	pos = 0;

	while(( str[pos] <= '9' ) && ( str[pos] >= '0' ))
	{
		pos++;
	}

	len = pos;

	if ( len > 10 )
	{
		len = 10;
	}

	cnt = 0;

	while( len-- )
	{
		retval += (( str[--pos] - '0' ) * ( _decnumbers[cnt++] ));
	}

bailout:

return( retval );
}

// --

SEC_CODE static U32 _GetHexadecimal( STR str )
{
U32 retval;

	retval = 0;

	if ( ! str )
	{
		goto bailout;
	}

	str += 2;

	while( HexData[ (U32) *str ] != -1 )
	{
		retval  = retval << 4;
		retval |= HexData[ (U32) *str++ ];
	}

bailout:

	return( retval );
}

// --

SEC_CODE static U32 ReadValue( STR mem )
{
U32 v;

	v = 0;

	if (( mem[0] == '0' ) && (( mem[1] == 'x' ) || ( mem[1] == 'X' )))
	{
		v = _GetHexadecimal( mem );
	}
	else
	{
		v = _GetDecimal( mem );
	}

	return( v );
}

// --

SEC_CODE struct USB2_FktDriverNode *__FDriver_Load( struct USBBase *usbbase, STR filename )
{
struct USB2_FktDriverNode *fdn;
struct TagItem Tags[_LAST+1];
struct ExamineData *ed;
struct DOSIFace *IDOS;
U32 argstart;
U32 nextline;
U32 cmdstart;
U32 cmdlen;
U32 cmdpos;
U32 size;
STR mem;
S32 dir;
U32 pos;
U32 len;
S32 cmp;
S32 chr;

	TASK_NAME_ENTER( "__FDriver_Load" );

	ed		= NULL;
	fdn		= NULL;
	mem		= NULL;
	dir		= 0;
	IDOS	= usbbase->usb_IDOS;

	if (( ! IDOS ) || ( ! filename ))
	{
		goto bailout;
	}

	dir = MISC_SETCURRENTDIR( usbbase->usb_DriverDirLock );

	// --

	ed = IDOS->ExamineObjectTags(
		EX_StringNameInput, filename,
		TAG_END
	);

	if ( ! ed )
	{
		USBDEBUG( "%s not found", filename );
		goto bailout;
	}

	// --
	// We now have a correct date

	for( pos=0 ; pos<_LAST ; pos++ )
	{	
		Tags[pos].ti_Tag	= TAG_IGNORE;
		Tags[pos].ti_Data	= 0;
	}
	
	Tags[ _FNClass	].ti_Tag	= (Tag) USB2Tag_FDriver_Class_Filename;
	Tags[ _FNClass	].ti_Data	= (Tag) filename;

//	Tags[ _Device	].ti_Tag	= (Tag) USB2Tag_FDriver_DeviceList;
//	Tags[ _Device	].ti_Data	= (Tag) & l;

//	Tags[ _Date 	].ti_Tag	= (Tag) USB2Tag_FDriver_DriverDate;
//	Tags[ _Date		].ti_Data	= (Tag) & ed->Date;

//	Tags[ _Class 	].ti_Tag	= (Tag) USB2Tag_FDriver_Class;
//	Tags[ _Class	].ti_Data	= (Tag) FDCLASS_Any;

//	Tags[ _SubClass ].ti_Tag	= (Tag) USB2Tag_FDriver_SubClass;
//	Tags[ _SubClass	].ti_Data	= (Tag) FDSUBCLASS_Any;

//	Tags[ _Protocol	].ti_Tag	= (Tag) USB2Tag_FDriver_Protocol;
//	Tags[ _Protocol	].ti_Data	= (Tag) FDPROTOCOL_Any;

	Tags[ _LAST		].ti_Tag	= (Tag) TAG_END;
	Tags[ _LAST		].ti_Data	= (Tag) TAG_END;

	// ---

	SEMAPHORE_OBTAIN( & usbbase->usb_FDriver_Semaphore );

	len = MISC_STRLENGTH( filename );
	fdn = usbbase->usb_FDriver_Header.uh_Head;

	while( fdn )
	{
		if (( fdn->fdn_Class_Filename )
		&&	( fdn->fdn_Class_FilenameLen == len )
		&&	( ! MISC_STRICASECOMPARE( fdn->fdn_Class_Filename, filename )))
		{
			break;
		}
		else
		{
			fdn = Node_Next( fdn );
		}
	}

	if ( fdn )
	{
		cmp = IDOS->CompareDates( & fdn->fdn_ClassDate, & ed->Date );
	}
	else
	{
		cmp = 1;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_FDriver_Semaphore );

	if ( cmp <= 0 )
	{
		goto bailout;
	}
	else
	{
		USBDEBUG( "Updateing %s", filename );
	}

	// ----

	pos = 0;
	mem = MISC_LOADFILE( filename, & size );

	if ( ! mem )
	{
		goto bailout;
	}

	while( mem[pos] )
	{
		// Skip space

		while(( mem[pos] == 9 ) || ( mem[pos] == 10 ) || ( mem[pos] == 13 ) || ( mem[pos] == 32 ))
		{
			pos++;
		}

		// Next Line

		nextline = pos;

		while(( mem[nextline] ) && ( mem[nextline] != 10 ) && ( mem[nextline] != 13 ))
		{
			nextline++;
		}

		while(( mem[nextline] == 10 ) || ( mem[nextline] == 13 ))
		{
			nextline++;
		}

		if ( mem[pos] != '#' )
		{
			// Count Command Length

			cmdstart = pos;

			while(( mem[pos] ) && ( mem[pos] != 9 ) && ( mem[pos] != 32 ))
			{
				pos++;
			}

			cmdlen = pos - cmdstart;

			// Find Argument start Pos

			while(( mem[pos] == 9 ) || ( mem[pos] == 32 ))
			{
				pos++;
			}

			// Find Command

			cmdpos = 0;

			while( Cmds[cmdpos].CmdName )
			{
				if (( Cmds[cmdpos].CmdLen == cmdlen )
				&&  ( MISC_STRINCASECOMPARE( & mem[cmdstart], Cmds[cmdpos].CmdName, cmdlen ) == 0 ))
				{
					break;
				}
				else
				{
					cmdpos++;
				}
			}

			switch( Cmds[cmdpos].CmdType )
			{
				case _Type:
				{
					if (( mem[pos] ) && ( mem[pos] != 10 ))
					{
						/**/ if ( ! MISC_STRINCASECOMPARE( "Interface", & mem[pos], 9 ))
						{
							Tags[_Type].ti_Tag  = USB2Tag_FDriver_Type;
							Tags[_Type].ti_Data = USB2Val_FDriver_Interface;
						}
						else if ( ! MISC_STRINCASECOMPARE( "Function", & mem[pos], 9 ))
						{
							Tags[_Type].ti_Tag  = USB2Tag_FDriver_Type;
							Tags[_Type].ti_Data = USB2Val_FDriver_Function;
						}
						else if ( ! MISC_STRINCASECOMPARE( "Device", & mem[pos], 6 ))
						{
							Tags[_Type].ti_Tag  = USB2Tag_FDriver_Type;
							Tags[_Type].ti_Data = USB2Val_FDriver_Device;
						}
					}
					break;
				}

				case _Class:
				{
					if (( mem[pos] ) && ( mem[pos] != 10 ))
					{
						Tags[_Class].ti_Tag  = USB2Tag_FDriver_Class;
						Tags[_Class].ti_Data = ReadValue( & mem[pos] );
					}
					break;
				}

				case _SubClass:
				{
					if (( mem[pos] ) && ( mem[pos] != 10 ))
					{
						Tags[_SubClass].ti_Tag  = USB2Tag_FDriver_SubClass;
						Tags[_SubClass].ti_Data = ReadValue( & mem[pos] );
					}
					break;
				}

				case _Protocol:
				{
					if (( mem[pos] ) && ( mem[pos] != 10 ))
					{
						Tags[_Protocol].ti_Tag  = USB2Tag_FDriver_Protocol;
						Tags[_Protocol].ti_Data = ReadValue( & mem[pos] );
					}
					break;
				}

//				case _Title:
//				case _Hotkey:
				case _FNDriver:
				{
					if (( mem[pos] ) && ( mem[pos] != 10 ))
					{
						/**/ if ( mem[pos] == '\'' )
						{
							chr = '\'';
							pos++;
						}
						else if ( mem[pos] == '\"' )
						{
							chr = '\"';
							pos++;
						}
						else if ( mem[pos] == '`' )
						{
							chr = '`';
							pos++;
						}
						else
						{
							chr = 10;
						}

						argstart = pos;

						// Find String end or Line end

						while(( mem[pos] ) && ( mem[pos] != 10 ) && ( mem[pos] != chr ))
						{
							pos++;
						}

						mem[pos] = 0;

						if ( pos != argstart )
						{
							/**/ if ( Cmds[cmdpos].CmdType == _Title )
							{
								Tags[_Title].ti_Tag  = USB2Tag_FDriver_Title;
								Tags[_Title].ti_Data = (U32) & mem[argstart];
							}
							else if ( Cmds[cmdpos].CmdType == _FNDriver )
							{
								Tags[_FNDriver].ti_Tag  = USB2Tag_FDriver_Driver_Filename;
								Tags[_FNDriver].ti_Data = (U32) & mem[argstart];
							}
//							else
//							{
//								Tags[_Hotkey].ti_Tag  = USB2Tag_FDriver_Hotkey;
//								Tags[_Hotkey].ti_Data = (U32) & mem[argstart];
//							}
						}
					}
					break;
				}

				default:
				{
					USBDEBUG( "CmdName '%s'", Cmds[cmdpos].CmdName );
					break;
				}
			}
		}

		pos = nextline;
	}

	// --

	fdn = FDRIVER_CREATELIST( Tags );

bailout:

	if (( IDOS ) && ( ed ))
	{
		IDOS->FreeDosObject( DOS_EXAMINEDATA, ed );
		ed = NULL;
	}

	if ( mem )
	{
		MEM_FREEVEC( mem );
		mem = NULL;
	}

	MISC_SETCURRENTDIR( dir );

	TASK_NAME_LEAVE();

	return( fdn );
}

// --
