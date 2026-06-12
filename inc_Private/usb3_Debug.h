
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_USB3_DEBUG_H
#define INC_PRIVATE_USB3_DEBUG_H

/***************************************************************************/

// --

#ifdef DO_PANIC

// we do a IOREQUEST_VALID(x) right after, if this is true
// okay we only use own RealRequester or TimeRequest
// if size it bigger than TimeRequest is proberly USB or garbage, so lets Validate it

#define IS_USB_IOREQ(x)		(	(x) \
	&& ((((struct RealRequest *)(x))->req_StructID==ID_USB3_IOR ) \
	||	(((struct RealRequest *)(x))->req_Public.io_Message.mn_Length > sizeof( struct TimeRequest ) )))

#endif

// --
#if 0

#ifdef DO_PANIC
void VARARGS68K _PANIC( STR file, U32 line, STR fmt, ... );
#define USBPANIC( fmt, ... )	_PANIC( __FILE__, __LINE__, fmt, ## __VA_ARGS__ )
#else
#define USBPANIC( fmt, ... )	((void)0)
#endif

// --

#if defined( DO_ERROR ) || defined( DO_INFO ) || defined( DO_DEBUG )

void VARARGS68K _USBDEBUG( U32 lvl, STR fmt, ... );

#ifdef DO_ERROR
#define USBERROR( fmt, ... )	_USBDEBUG( 2, fmt, ## __VA_ARGS__ )
#else
#define USBERROR( fmt, ... )	((void)0)
#endif

#ifdef DO_INFO
#define USBINFO( fmt, ... )		_USBDEBUG( 1, fmt, ## __VA_ARGS__ )
#else
#define USBINFO( fmt, ... )		((void)0)
#endif

#ifdef DO_DEBUG
#define USBDEBUG( fmt, ... )	_USBDEBUG( 0, fmt, ## __VA_ARGS__ )
#else
#define USBDEBUG( fmt, ... )	((void)0)
#endif

#else

 #define USBERROR( fmt, ... )	((void)0)
 #define USBINFO(  fmt, ... )	((void)0)
 #define USBDEBUG( fmt, ... )	((void)0)

#endif
#endif


#ifdef DO_PANIC
#define USBPANIC( fmt, ... )	usbbase->_Panic( usbbase, __FILE__, __LINE__, fmt, ##__VA_ARGS__ )
#else
#define USBPANIC( fmt, ... )	((void)0)
#endif

#ifdef DO_ERROR
#define USBERROR( fmt, ... )	usbbase->_Error( usbbase, fmt, ##__VA_ARGS__ )
#else
#define USBERROR( fmt, ... )	((void)0)
#endif

#ifdef DO_INFO
#define USBINFO( fmt, ... )		usbbase->_Info( usbbase, fmt, ##__VA_ARGS__ )
#else
#define USBINFO( fmt, ... )		((void)0)
#endif

#ifdef DO_DEBUG
#define USBDEBUG( fmt, ... )	usbbase->_Debug( usbbase, fmt, ##__VA_ARGS__ )
#else
#define USBDEBUG( fmt, ... )	((void)0)
#endif

// --

/***************************************************************************/

#endif // INC_PRIVATE_USB3_DEBUG_H
