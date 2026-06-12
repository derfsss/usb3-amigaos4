
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_VERSION_H
#define INC_VERSION_H

// --

#if defined(_USB3_)
#define DEVNAME		"usb3_printer.device"
#elif defined(_SIRION_)
#define DEVNAME		"rUSB_Printer.device"
#else
#error Unsupported USB Target
#endif

#define VERSION		1
#define REVISION	1
#define DATE		"27.8.2025"
#define VERS		DEVNAME " 1.1"
#define VSTRING		DEVNAME " 1.1 (" DATE ")\r\n"
#define VERSTAG		"\0$VER: " DEVNAME " 1.1 (" DATE ")"

// --

#endif
