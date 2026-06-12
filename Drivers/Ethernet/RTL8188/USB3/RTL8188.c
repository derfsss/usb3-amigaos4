/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --

/*
	bsd
	if_urtwn

	https://github.com/openbsd/src/blob/master/sys/dev/usb/if_urtwn.c
	https://lab.knightsofnii.com/kristaba/openbsd/-/blob/9ee53e869876be6a6d019c35178223271edf7c80/sys/dev/usb/if_urtwn.c
	https://socsvn.freebsd.org/socsvn/mirror/FreeBSD/projects/bsd_rdma_4_9_stable_11/sys/dev/urtwn/if_urtwn.c

*/

// --

struct Library *		DOSBase			= NULL;
struct DOSIFace *		IDOS			= NULL;

struct Library *		SysBase			= NULL;
struct ExecIFace *		IExec			= NULL;

struct Library *		NewlibBase		= NULL;
struct Interface *		INewlib			= NULL;

struct Library *		UtilityBase		= NULL;
struct UtilityIFace *	IUtility		= NULL;

// --

struct MsgPort *		USB3MsgPort		= NULL;
struct IORequest *		USB3IOReq		= NULL;
struct Device *			USB3Base		= NULL;
struct USB3_IFace *		IUSB3			= NULL;

// --
