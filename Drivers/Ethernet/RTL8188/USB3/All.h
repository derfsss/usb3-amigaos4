
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef ETH_DEVICE_H
#define ETH_DEVICE_H

// --

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/usb3.h>
#include <proto/utility.h>

#include <devices/newstyle.h>
#include <devices/parallel.h>
#include <dos/dostags.h>

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

// --

#include "Types.h"
#include "Version.h"
// --
#include "Base.h"
#include "Task.h"
#include "Debug.h"
//#include "Buffer.h"
//#include "Config.h"
//#include "RTL8188.h"

// --

#define NR_of_TXs	3

struct EthernetUnit
{
	struct Unit						unit_Unit;
	U32								unit_UnitNr;

	STR								unit_TaskName;
	enum TaskState					unit_TaskState;
	struct Task *					unit_TaskAdr;

	struct Task *					unit_ExitParent;
	U32								unit_ExitMask;

	struct MsgPort *				unit_ETH_Begin_MsgPort;
	U32								unit_ETH_Begin_MsgPortBit;

	struct MsgPort *				unit_ETH_Abort_MsgPort;
	U32								unit_ETH_Abort_MsgPortBit;

	struct USB3_Register *			unit_USB_Register;
	struct USB3_Interface *			unit_USB_Interface;

	struct USB3_IORequest *			unit_Bulk_Tx_Active[NR_of_TXs];	// Active USB3_IORequest's
	struct USB3_EPResource *		unit_Bulk_Tx_Resource;

	// struct USB3_IORequest *
	struct List *					unit_USB_IOReq_FreeList;		// Unused USB IOReq's

	// Active USB IOReq's
	S32								unit_USB_IOReq_ActiveCount;		//

	// struct IOExtPar *
	struct List *					unit_PAR_IOReq_WaitingList;		// Waiting too be send to usb printer
	S32								unit_PAR_IOReq_WaitingCount;	//

	// struct PrtBuffer * - we are filling now
	struct PrtBuffer *				unit_Buffer_Active;				// 

	// struct PrtBuffer * - Unused buffers after we have send to usb
	struct List *					unit_Buffer_FreeList;			// 
	S32								unit_Buffer_FreeCount;			// 

	// struct PrtBuffer *
	U32								unit_Buffer_Size;				// User set, default 16kb
	struct List *					unit_Buffer_WaitingList;		// Waiting too be send to usb printer
	S32								unit_Buffer_WaitingCount;		// 


	// -- Config
	U32								unit_USB_VendorID;
	U32								unit_USB_DeviceID;
//	struct ConfigOptions			unit_Options;

	// -- Settings
	U8								unit_StartupComplete;
	U8								unit_NotRunning;			// Error shown
	U8								unit_CfgError;				// Cfg err Shown
	U8								unit_Detached;				// USB Device disconnected
	U8								unit_EthernetStatus;
	U8								unit_LastByteSend;
	U8								unit_Shutdown;				// Yes time to stop, clear buffers and exit
	U8								unit_Running;				// 
};

// --


// --

extern struct IORequest *USB3IOReq;
extern struct MsgPort *USB3MsgPort;

// --

extern struct Interface *INewlib;
extern struct Library *NewlibBase;

struct EthernetUnit *Unit_Alloc( U32 unitnr );

//void	Cmd_Write_Buffer(		struct EthernetUnit *unit, struct IOExtPar *pario );
//void	Cmd_Write_Direct(		struct EthernetUnit *unit, struct IOExtPar *pario );
S32		Resources_Init(			struct EthernetBase *ethBase );
void	Resources_Free(			struct EthernetBase *ethBase );
//void	Buffer_Flush(			struct EthernetUnit *unit );
//S32	Buffer_Write(			struct EthernetUnit *unit, U8 *data, U32 size );
//void	Buffer_Schedule(		struct EthernetUnit *unit );
//void	Direct_Schedule(		struct EthernetUnit *unit );
S32		Process_Find_Device(	struct EthernetUnit *unit );
S32		Process_Setup_Device(	struct EthernetUnit *unit );
S32		Unit_Startup(			struct EthernetUnit *unit );
S32		Unit_Startup_USB(		struct EthernetUnit *unit );
void	Unit_Free(				struct EthernetUnit *unit );
void	NSCmd_DeviceQuery(		struct IOStdReq *ioreq );
void	Ethernet_Process_Entry(	void );

// --

#endif
