
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_ALL_H
#define INC_ALL_H

// --

// #include <proto/application.h>
// #include <proto/asl.h>
// #include <proto/button.h>
// #include <proto/chooser.h>
// #include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/exec.h>
// #include <proto/graphics.h>
#include <proto/intuition.h>
// #include <proto/label.h>
// #include <proto/layout.h>
// #include <proto/Picasso96API.h>
// #include <proto/slider.h>
// #include <proto/space.h>
#include <proto/usb3.h>
#include <proto/utility.h>

// #undef __USE_INLINE__
// // work around as inline4/blankermodule.h is missing from my sdk
// #include <proto/blankermodule.h>
// #define __USE_INLINE__

// #include <proto/screenblanker.h>

// #include <classes/window.h>
// #include <cybergraphx/cybergraphics.h>
// #include <gadgets/button.h>
// #include <gadgets/chooser.h>
// #include <gadgets/layout.h>
// #include <gadgets/slider.h>
// #include <gadgets/space.h>
// #include <images/label.h>
// #include <intuition/icclass.h>
// #include <intuition/pointerclass.h>
// #include <libraries/blankermodule.h>
// #include <libraries/screenblanker.h>

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
// #include <stdio.h>

// --
#include "inc/Types.h"
#include "inc/Version.h"
// --
#include "inc/Base.h"
#include "inc/Command.h"
#include "inc/Debug.h"
#include "inc/FileSystem.h"
#include "inc/Master.h"
#include "inc/PTP.h"
#include "inc/USB.h"
// --
#include "inc/Protos.h"
// --

#endif
