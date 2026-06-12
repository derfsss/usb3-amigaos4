
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#define INC_PRIVATE_USB3_ALL_H

/***************************************************************************/

#define __NOLIBBASE__
#define __NOGLOBALIFACE__

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/input.h>
#include <proto/intuition.h>
#include <proto/mounter.h>
#include <proto/usb3.h>
#include <proto/utility.h>
#include <proto/timer.h>

#include <devices/input.h>
#include <devices/newstyle.h>
#include <devices/scsidisk.h>
#include <devices/trackdisk.h>
#include <devices/hardblocks.h>

#include "scsi/scsi.h"
#include "scsi/devtypes.h"

#include <stdarg.h>
#include <stddef.h>

// --

#include "inc_Private/usb3_IDs.h"
#include "inc_Private/usb3_Debug.h"
#include "inc_Private/usb3_Node.h"
#include "inc_Private/usb3_Signal.h"
#include "inc_Private/usb3_Semaphore.h"
#include "inc_Private/usb3_MsgPort.h"
#include "inc_Private/usb3_ASync.h"
#include "inc_Private/usb3_Misc.h"

#include "inc_Private/Command.h"
#include "inc_Private/EndPoint.h"
#include "inc_Private/Function.h"
#include "inc_Private/Interface.h"
#include "inc_Private/Config.h"
#include "inc_Private/Task.h"
#include "inc_Private/HCD_EHCI.h"
#include "inc_Private/HCD_OHCI.h"
#include "inc_Private/HCD_UHCI.h"
#include "inc_Private/HCD_XHCI.h"
#include "inc_Private/HCD.h"
#include "inc_Private/HID.h"
#include "inc_Private/HUB.h"
#include "inc_Private/Memory.h"
#include "inc_Private/Notify.h"
#include "inc_Private/IORequest.h"
#include "inc_Private/Register.h"
#include "inc_Private/Device.h"
#include "inc_Private/FDriver.h"
#include "inc_Private/Driver.h"

// --

#define DRIVERDIR		"libs:USB3"

// --

#ifdef DO_PANIC

#define TASK_NAME_ENTER(x)	\
	PTR UNUSED ____OLD = ((struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface)->FindTask(NULL)->tc_Node.ln_Name; \
	((struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface)->FindTask(NULL)->tc_Node.ln_Name=(PTR)(x);

#define TASK_NAME_SET(x)	\
	((struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface)->FindTask(NULL)->tc_Node.ln_Name=(PTR)(x);

#define TASK_NAME_LEAVE()		\
	((struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface)->FindTask(NULL)->tc_Node.ln_Name=____OLD;

#else

#define TASK_NAME_ENTER(x)
#define TASK_NAME_SET(x)
#define TASK_NAME_LEAVE()

#endif

#ifdef SECTION_NAME
	#define SEC_CODE	__attribute__(( section( ".text." SECTION_NAME )))
//	#define SEC_RWDATA	__attribute__(( section( ".data." SECTION_NAME )))
//	#define SEC_RODATA	__attribute__(( section( ".rodata." SECTION_NAME )))

//	data do not have a problem with size, can easy fit.. so put into same sections
	#define SEC_RWDATA	__attribute__(( section( ".data" )))
	#define SEC_RODATA	__attribute__(( section( ".rodata" )))
#else
	#define SEC_CODE
	#define SEC_RWDATA
	#define SEC_RODATA
#endif

// --
// Valid Status (Test)
// for xx_VALID() functions

enum VSTAT
{
	VSTAT_Error,			// Invalid Structure Pointer
	VSTAT_Null,				// NULL Pointer
	VSTAT_Okay,				// Valid Struct
};

// --
// Lock Status
// for xx_LOCK() functions

enum LSTAT
{
	LSTAT_Error,			// Invalid Structure Pointer
	LSTAT_Null,				// NULL Pointer, nothing done
	LSTAT_Okay,				// We locked Struct
};

// --
// Free Status
// for xx_FREE() functions

enum FSTAT
{
	FSTAT_Error,			// Invalid Structure Pointer
	FSTAT_Null,				// NULL Pointer, nothing done
	FSTAT_Locked,			// Unable to Free, proberly locked
	FSTAT_Okay,				// We were able to Free Structure
};

// --
// FDriver Stat

enum FDSTAT
{
	FDSTAT_Error,
	FDSTAT_Not_Found,
	FDSTAT_Found,
};

// --
// Device Commands

#define CMD_AddFunction			256		// Internal Command
#define CMD_RemFunction			257		// Internal Command

struct CMD_AddFunctionData
{
	struct RealFunctionNode *	Parent;
	U32							Speed;
	U32							Port;
	struct RealFunctionNode *	Result;
};

// --
// Build Protos

#define USB2_CREATEFUNC(ret_type, name, ...)	ret_type __##name(__VA_ARGS__)
#include "usb3_Protos.h"
#undef USB2_CREATEFUNC

// --

/***************************************************************************/

#endif
