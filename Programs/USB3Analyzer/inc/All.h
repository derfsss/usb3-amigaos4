
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_ALL_H
#define INC_ALL_H

/***************************************************************************/

#define __USE_INLINE__

#include <proto/asl.h>
#include <proto/button.h>
#include <proto/dos.h>
#include <proto/exec.h>
// #include <proto/expansion.h>
#include <proto/graphics.h>
// #include <proto/input.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <proto/listbrowser.h>
// #include <proto/mounter.h>
#include <proto/usb3.h>
#include <proto/utility.h>
// #include <proto/timer.h>
#include <proto/window.h>

#include <classes/window.h>
#include <gadgets/button.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// --

#define End			TAG_END)

// --

#include "inc/Types.h"
#include "inc/Version.h"
// --
#include "inc/Screen.h"
#include "inc/USB.h"
// --
#include "inc/Protos.h"
#include "inc/Externals.h"

/***************************************************************************/

#endif
