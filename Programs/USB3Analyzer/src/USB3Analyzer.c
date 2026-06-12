
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

const char		myVersion[]		= VERSTAG;
struct Task *	Main_Task		= NULL;
S32				Restart			= FALSE;
S32				Running			= FALSE;

// --

static S32	Main_Alloc( void );
static S32	Main_Start( void );
static void Main_Stop( void );
static void Main_Free( void );
static void Main_Loop( void );

// --

int main( void )
{
S32 retval;

	retval = RETURN_FAIL;

	if ( Main_Alloc() )
	{
		do
		{
			Restart = FALSE;

			if ( Main_Start() )
			{
				Main_Loop();
			}

			Main_Stop();
		}
		while( Restart );

		retval = RETURN_OK;
	}

	Main_Free();

	return( retval );
}

// --

static S32 Main_Alloc( void )
{
S32 retval;

	retval = FALSE;

	// Open Libraries

	Main_Task = FindTask( NULL );

	NewList( & DeviceTreeUSBList );
	NewList( & DeviceTreeGUIList );
	NewList( & PropertiesGUIList );
	NewList( & IORequestGUIList );
	NewList( & CaptureGUIList );

	if ( ! myOpenResources( 16384 ))
	{
		printf( "myOpenResources() failed\n" );
		goto bailout;
	}

	if ( ! USB_Setup() )
	{
		printf( "USB_Setup() failed\n" );
		goto bailout;
	}

//	if ( Main_OpenTimerDevice() == FALSE )
//	{
//		goto bailout;
//	}

//	if ( Main_CreateMsgPorts() == FALSE )
//	{
//		goto bailout;
//	}

//	if ( Load_Program_Settings() == FALSE )
//	{
//		goto bailout;
//	}

//	if ( Init_Databases() == FALSE )
//	{
//		goto bailout;
//	}

//	if ( Main_Init1() == FALSE )
//	{
//		goto bailout;
//	}

	if ( ! Main_BuildWindow() )
	{
		printf( "Main_BuildWindow() failed\n" );
		goto bailout;
	}

	retval = TRUE;

bailout:

	return(	retval );
}

// --

static void Main_Free( void )
{
//	Main_Free1();

	Main_DestoryWindow();

//--	Free_Databases();

//--	Main_CloseTimer();

//	if ( PHandle )
//	{
//		IUSB3Config->USBEnv_Delete( PHandle );
//		PHandle = NULL;
//	}

//	Main_DeleteMsgPorts();

//	Main_CloseTimerDevice();

	USB_Cleanup();

	myCloseResources();
}

// --

static S32 Main_Start( void )
{
S32 retval;

	retval = FALSE;

	if ( ! Main_OpenScreen() )
	{
		printf( "Main_OpenScreen() failed\n" );
		goto bailout;
	}

	if ( ! Main_OpenWindow() )
	{
		printf( "Main_OpenWindow() failed\n" );
		goto bailout;
	}

//	Main_StartTimer();

	retval = TRUE;

bailout:

	return(	retval );
}

// --

static void Main_Stop( void )
{
//	Main_RemNotify();

//	Prefs_CloseWindow();

	Main_CloseWindow();

	Main_CloseScreen();
}

// --

static void Main_Loop( void )
{
U32 mask;

	Running = TRUE;

	while( Running )
	{
		mask = Wait(
//			CmdMsgPortBit |
			Main_WindowBits |
//			DBBrowserWindowBits |
//			DBInfoWindowBits |
//			MemWindowBits |
//			PrefsWindowBits |
//			TimerMsgPortBit |
//			USBNotifyMsgPortBit |
//			USBWatchMsgPortBit |
			USBNotifyBit |
			SIGBREAKF_CTRL_C
		);

		if ( mask & Main_WindowBits )
		{
			Main_HandleWindow();
		}

//		if ( mask & DBBrowserWindowBits )
//		{
//			DBBrowser_HandleWindow();
//		}
//
//		if ( mask & DBInfoWindowBits )
//		{
//			DBInfo_HandleWindow();
//		}

//		if ( mask & MemWindowBits )
//		{
//			Mem_HandleWindow();
//		}

//		if ( mask & PrefsWindowBits )
//		{
//			Prefs_HandleWindow();
//		}

//		if ( mask & TimerMsgPortBit )
//		{
//			Handle_Timer();
//		}

//		if ( mask & CmdMsgPortBit )
//		{
//			Handle_Commands();
//		}

//		if ( mask & USBNotifyMsgPortBit )
//		{
//			Handle_Notify();
//		}

//		if ( mask & USBWatchMsgPortBit )
//		{
//			Handle_USBWatch();
//		}

		if ( mask & USBNotifyBit )
		{
			Handle_Notify();
		}

		if ( mask & SIGBREAKF_CTRL_C )
		{
			Running = FALSE;
		}
	}
}

// --
