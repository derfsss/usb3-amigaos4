
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "inc/All.h"

// --

#define WIN_MINWIDTH	1024
#define WIN_MINHEIGHT	768


struct Screen *		Main_ScreenAdr		= NULL;
enum SCRTYPE		Main_ScreenType		= SCRTYPE_Screen;

// --

S32 Main_OpenScreen( void )
{
struct ScreenModeRequester *modereq;
struct Screen *scr;
S32 retval;
U32 type;
U32 mode;
U32 ret;
U32 w;
U32 h;

	retval = FALSE;

	type = Prefs_ScrType;
	mode = 0;
	scr = NULL;
	w = 0;
	h = 0;

	// Window

	if ( type == SCRTYPE_Window )
	{
		scr = LockPubScreen( "Workbench" );

		if ( ! scr )
		{
			// Type to Clone WB
			type = SCRTYPE_WBClone;
		}
	}

	// Workbech Clone

	if ( type == SCRTYPE_WBClone )
	{
		scr = OpenScreenTags( NULL,
			SA_WindowDropShadows,	FALSE,
			SA_LikeWorkbench,   	TRUE,
			SA_PubName,				VERS,
			SA_Title,				VERS,
			TAG_END
		);

		if ( ! scr )
		{
			// Custom Screen
			type = SCRTYPE_Screen;
		}
	}

	// Custom Screen

	if ( type == SCRTYPE_Screen )
	{
		mode= Prefs_ScrMode;
		w	= Prefs_ScrWidth;
		h	= Prefs_ScrHeight;

		if ( ModeNotAvailable( mode ) == INVALID_ID )
		{
			modereq = AllocAslRequestTags( ASL_ScreenModeRequest,
				TAG_END
			);

			if ( modereq )
			{
				ret = AslRequestTags( modereq,
					ASLSM_InitialDisplayID,	mode,
					ASLSM_DoWidth,			TRUE,
					ASLSM_DoHeight,			TRUE,
					ASLSM_MinWidth,			WIN_MINWIDTH,
					ASLSM_MinHeight,		WIN_MINHEIGHT,
					TAG_END
				);

				if ( ret )
				{
					mode		= modereq->sm_DisplayID;
					w			= modereq->sm_DisplayWidth;
					h			= modereq->sm_DisplayHeight;

					Prefs_ScrWidth	= w;
					Prefs_ScrHeight	= h;
					Prefs_ScrMode	= mode;

//					IUSB3Config->USBEnv_SetHex( PHandle, "Screen",	 "ModeID",	 mode );
//					IUSB3Config->USBEnv_SetDec( PHandle, "Width",	 "Width", 	 w );
//					IUSB3Config->USBEnv_SetDec( PHandle, "Width",	 "Height", 	 h );
//					IUSB3Config->USBEnv_Use( PHandle );
				}

				FreeAslRequest( modereq );
			}
		}

		if ( w < WIN_MINWIDTH )
		{
			w = WIN_MINWIDTH;
		}

		if ( h < WIN_MINHEIGHT )
		{
			h = WIN_MINHEIGHT;
		}

		scr = OpenScreenTags( NULL,
			SA_WindowDropShadows,	FALSE,
			SA_AutoScroll,			TRUE,
			SA_LikeWorkbench,		TRUE,
			SA_Overscan,			OSCAN_STANDARD,
			SA_PubName,				VERS,
			SA_Title,				VERS,
			SA_Type,				CUSTOMSCREEN,
			SA_DisplayID,			mode,
			SA_Width,				w,
			SA_Height,				h,
			TAG_END
		);
	}

	if ( ! scr )
	{
		printf( "failed to open screen\n" );
 		goto bailout;
	}

	Main_ScreenAdr = scr;
	Main_ScreenType = type;
	Prefs_ScrType = type;

	switch( type )
	{
		case SCRTYPE_Window:
		{
			if ( Main_WinWidth < WIN_MINWIDTH )
			{
				Main_WinWidth = WIN_MINWIDTH;
			}
	
			if ( Main_WinHeight < WIN_MINHEIGHT )
			{
				Main_WinHeight = WIN_MINHEIGHT;
			}
	
			SetAttrs( Main_WinObject,
				WA_Backdrop,		FALSE,
				WA_Borderless,		FALSE,
				WA_CloseGadget,		TRUE,
				WA_CustomScreen,	scr,
				WA_DepthGadget,		TRUE,
				WA_DragBar,			TRUE,
				WA_SizeGadget,		TRUE,
				WA_Title,			VERS " (" DATE ")",

				// Set last win pos
				WA_Top,				Main_WinYPos,
				WA_Left,			Main_WinXPos,
				WA_Width,			Main_WinWidth,
				WA_Height,			Main_WinHeight,

				TAG_END
			);
			break;
		}

		case SCRTYPE_Screen:
		case SCRTYPE_WBClone:
		{
			SetAttrs( Main_WinObject,
				WA_Backdrop,		TRUE,
				WA_Borderless,		TRUE,
				WA_CloseGadget,		FALSE,
				WA_CustomScreen,	scr,
				WA_DepthGadget,		FALSE,
				WA_DragBar,			FALSE,
				WA_SizeGadget,		FALSE,
				WA_Title,			NULL,

				// Move Window under Screen Title Bar
				WA_Top,				scr->WBorTop + scr->Font->ta_YSize,
				WA_Left,			0,
				WA_Width,			scr->Width,
				WA_Height,			scr->Height - ( scr->WBorTop + scr->Font->ta_YSize ),

				TAG_END
			);
			break;
		}

		default:
		{
			break;
		}
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --

void Main_CloseScreen( void )
{
	if ( Main_ScreenAdr )
	{
		if ( Main_ScreenType == SCRTYPE_Window )
		{
			UnlockPubScreen( "Workbench", Main_ScreenAdr );
		}
		else
		{
			CloseScreen( Main_ScreenAdr );
		}

		Main_ScreenAdr = NULL;
	}

}

// --
