
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"
#include "Master.h"

// --

/*
** Scan Driver Dir ( libs:usb3/ ) for extrenal classes
** Skipping known.. they have a file notify
**
*/

SEC_CODE static void __myProcess_ScanDriverDir( struct USBBase *usbbase, struct intern *in UNUSED )
{
struct USB3_FktDriverNode *fdn;
// struct NotifyMessage *msg;
struct ExamineData *dat;
struct DOSIFace *IDOS;
char parse[64];
STR pattern;
S32 oldproc;
S32 oldlock;
PTR ctx;

	// --

 	ctx	= NULL;
 	IDOS = usbbase->usb_IDOS;

	// --
	// Validate Dir lock

	if ( ! usbbase->usb_DriverDirLock )
	{
		return;
	}

	// --
	// Prevent DOS opening any windows

	oldproc = MISC_SETPROCWINDOW( -1 );
	oldlock = MISC_SETCURRENTDIR( usbbase->usb_DriverDirLock );

	// --

	pattern = "#?.class";

	if ( IDOS->ParsePatternNoCase( pattern, parse, sizeof(parse) ) == -1 )
	{
		goto bailout;
	}

	ctx = IDOS->ObtainDirContextTags(
		EX_LockInput,	usbbase->usb_DriverDirLock,
		EX_MatchString, parse,
		EX_DataFields,	EXF_TYPE|EXF_NAME,
		TAG_END
	);

	while(( dat = IDOS->ExamineDir( ctx )))
	{
		if ( EXD_IS_FILE( dat ))
		{
 			SEMAPHORE_OBTAIN( & usbbase->usb_FDriver_Semaphore );

			fdn = usbbase->usb_FDriver_Header.uh_Head;

			while( fdn )
			{
				if ( ! MISC_STRICASECOMPARE( fdn->fdn_Class_Filename, dat->Name ))
				{
					break;
				}
				else
				{
					fdn = Node_Next( fdn );
				}
			}

			// If the File is not found, then its new and we need to load it
			// If the File is found, then we will handle it via FileNotify
			if ( ! fdn )
			{
				fdn = FDRIVER_LOAD( dat->Name );

// 				#if 0
// 				if ( fdn )
// 				{
// 					struct bindmessage msg;

// 					msg.Task = usbbase->usb_IExec->FindTask( NULL );
// 					msg.usbbase = usbbase;
// 					msg.fdn = fdn;

// 					if ( MISC_STARTPROCESS(
// 						NP_Name,	    "Universal Serial Bus",
// 						NP_Child,	    TRUE,
// 						NP_Priority,    10,
// 						NP_StackSize,   20*1024,
// 						NP_Entry,	    Bind_Process,
// 						NP_UserData,    &msg,
// 						TAG_END ))
// 					{
// 						usbbase->usb_IExec->Wait( SIGBREAKF_CTRL_E );
// 					}
// 				}
// 				#endif

			}

			SEMAPHORE_RELEASE( & usbbase->usb_FDriver_Semaphore );
		}
	}

//	  if ( ERROR_NO_MORE_ENTRIES != IDOS->IoErr() )
//	  {
//		  goto bailout;
//	  }

bailout:

	if ( ctx )
	{
		IDOS->ReleaseDirContext( ctx );
	}

	MISC_SETCURRENTDIR( oldlock );
	MISC_SETPROCWINDOW( oldproc );
}
