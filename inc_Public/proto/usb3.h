
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PROTO_USB3_H
#define PROTO_USB3_H

/****************************************************************************/

#ifndef DEVICES_USB3_H
#include <devices/usb3.h>
#endif

/****************************************************************************/

#ifdef __amigaos4__

  #include <interfaces/usb3.h>

  #ifdef __USE_INLINE__
  #include <inline4/usb3.h>
  #endif

  #ifndef CLIB_USB2_PROTOS_H
  #define CLIB_USB2_PROTOS_H 1
  #endif

  #ifndef __NOLIBBASE__
  extern struct Device *USB2Base;
  #endif

  #ifndef __NOGLOBALIFACE__
  extern struct USB2IFace *IUSB2;
  #endif

#else // Other platforms

  #error USB2 is only supported on AmigaOS4 .. at the moment

#endif

/****************************************************************************/

#endif
