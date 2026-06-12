
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
** Copyright (c) 2001 The NetBSD Foundation, Inc.
** All rights reserved.
**
** This code is derived from software contributed to The NetBSD Foundation
** by Lennart Augustsson (lennart@augustsson.net).
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. All advertising materials mentioning features or use of this software
**    must display the following acknowledgement:
**        This product includes software developed by the NetBSD
**        Foundation, Inc. and its contributors.
** 4. Neither the name of The NetBSD Foundation nor the names of its
**    contributors may be used to endorse or promote products derived
**    from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
** ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

// --

#include "usb3_all.h"
#include "OHCI.h"

// --
#if defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void OHCI_Dump_Setup( struct USB3_HCDNode *hn, struct USB3_SetupData *sd )
{
struct ExecIFace *IExec;
STR typeText;
//U32 request_type = setup->sd_RequestType;
U32 request = sd->RequestCode;
U32 value = LE_SWAP16( sd->Value );
U32 index = LE_SWAP16( sd->Index );
U32 length = LE_SWAP16( sd->Length );
//U32 recipient = request_type & USBSDTM_RECIPIENT;

	static CONST STR request_string[] = {
/*  0 */"GET_STATUS",
/*  1 */"CLEAR_FEATURE",
/*  2 */"UNKNOWN",
/*  3 */"SET_FEATURE",
/*  4 */"UNKNOWN",
/*  5 */"SET_ADDRESS",
/*  6 */"GET_DESCRIPTOR",
/*  7 */"SET_DESCRIPTOR",
/*  8 */"GET_CONFIGURATION",
/*  9 */"SET_CONFIGURATION",
/* 10 */"GET_INTERFACE",
/* 11 */"SET_INTERFACE",
/* 12 */"SYNCH_FRAME"
	};

	static CONST STR type_string[] = {
		"STANDARD",
		"CLASS",
		"VENDOR",
		"UNKNOWN",
	};

	static CONST STR rec_string[] = {
		"DEVICE",
		"INTERFACE",
		"ENDPOINT",
		"OTHER"
	};

	IExec = hn->hn_USBBase->usb_IExec;

	{
		STR str1;
		STR str2;
		STR str3;
		U32 val1;
		U32 val2;
		U32 val3;
		U32 val4;

		val1 = ( sd->RequestType & 0x80 );
		val2 = ( sd->RequestType & 0x70 ) >> 5;
		val3 = ( sd->RequestType & 0x0f );
		val4 = ( sd->RequestType );

		str1 = val1 ? "READ" : "WRITE";
		str2 = type_string[ val2 ];
		str3 = ( val3 < 4 ) ? rec_string[ val3 ] : "UNKNOWN";

		IExec->DebugPrintF( "RequestType..... %s | %s | %s (%02lx)\n", str1, str2, str3, val4 );
	}

	{
		STR str1;
		U32 val1;

		val1 = ( sd->RequestCode );

		str1 = ( val1 > 12 ) ? "UNKNOWN" : request_string[ val1 ];


		IExec->DebugPrintF( "Requestcode..... %s (%02lx)\n", str1, val1 );
	}

	typeText = "";

	switch( request )
	{
		case REQCODE_Get_Descriptor:
		{
			switch( value >> 8 )
			{
				case DSCTYPE_Device:		typeText = "Device"; 			break;
				case DSCTYPE_Configuration:	typeText = "Configuration"; 	break;
				case DSCTYPE_String:		typeText = "String"; 			break;
				case DSCTYPE_Interface:		typeText = "Interface"; 		break;
				case DSCTYPE_EndPoint:		typeText = "Endpoint";			break;
				case 0x06:					typeText = "Device Qualifier";	break;
				case 0x07:					typeText = "Other Speed Config";break;
				case 0x08:					typeText = "Interface Power";	break;
				case 0x21:					typeText = "HID";				break;
				case 0x22:					typeText = "HID Report";		break;
				case 0x29:					typeText = "HUB Report";		break;
				default:					typeText = "Unknown";			break;
			}
			break;
		}
		break;
	}

	IExec->DebugPrintF( "Value........... %04lx (%s)\n", value, typeText );

	IExec->DebugPrintF( "Index........... %04lx\n", index );

	IExec->DebugPrintF( "Length.......... %04lx\n", length );
}

#endif
// --
