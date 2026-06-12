
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef __COMPPRO_H__
#define __COMPPRO_H__

// --

#define __USE_INLINE__
#define __USE_BASETYPE__

#include <proto/dos.h>
#include <proto/exec.h>

#include <inttypes.h>

// --
#if 1

#ifndef U8
typedef uint8_t U8;
#endif

#ifndef U16
typedef uint16_t U16;
#endif

#ifndef U32
typedef uint32_t U32;
#endif

#ifndef U64
typedef uint64_t U64;
#endif

#ifndef S8
typedef int8_t S8;
#endif

#ifndef S16
typedef int16_t S16;
#endif

#ifndef S32
typedef int32_t S32;
#endif

#ifndef S64
typedef int64_t S64;
#endif

#ifndef PTR
typedef void* PTR;
#endif

#ifndef STR
typedef char* STR;
#endif

#endif
// --



#endif

// --
