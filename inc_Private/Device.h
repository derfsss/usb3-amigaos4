
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_DEVICE_H
#define INC_PRIVATE_DEVICE_H

/***************************************************************************/

// Not really used (I think), only pointed too
struct USB3_DeviceUnit
{
	struct Unit						unit_Unit;
};

struct USBBase
{
	struct Library					usb_Library;
	U16								usb_Pad;
	PTR								usb_SegList;

	/*
	**
	*/

	// --

	struct ExecIFace *				usb_IExec;
	struct Library *				usb_DOSBase;
	struct Library *				usb_UtilityBase;
	struct Library *				usb_ExpansionBase;
	struct Library *				usb_IntuitionBase;
	struct Library *				usb_MounterBase;

	struct PCIIFace *				usb_IPCI;
	struct MMUIFace *				usb_IMMU;
	struct DOSIFace *				usb_IDOS;
	struct USB3IFace *				usb_IUSB3;
	struct UtilityIFace *			usb_IUtility;
	struct ExpansionIFace *			usb_IExpansion;
	struct IntuitionIFace *			usb_IIntuition;
	struct MounterIFace *			usb_IMounter;
	struct MounterPrivateIFace *	usb_IMounterPriv;

	// --

	U32								usb_CPU_PageSize;
	U32								usb_CPU_CacheSize;
	U32								usb_CPU_Type;
	U32								usb_MachineType;

	struct USB3_DeviceUnit			usb_OpenUnit;
	struct USB3_Semaphore			usb_OpenCloseSemaphore;

	struct USB3_Semaphore			usb_LockSemaphore;

	struct USB3_Header				usb_HCDHeader;				// struct USB3_HCDNode's
	struct USB3_ASync				usb_HCDASync;

	struct MSDBase *				usb_MSDDevice;
//	struct USB3_Header				usb_MSDHeader;
//	struct USB3_Semaphore			usb_MSDSemaphore;
//	struct USB3_Header				usb_MSDDiskChangeList;

	USB3_ID							usb_Notify_ID;
	struct USB3_Header				usb_Notify_Header;
	struct USB3_Semaphore			usb_Notify_Semaphore;
	struct USB3_MsgPort * 			usb_Notify_ReplyMsgPort;

	struct TimerIFace *				usb_ITimer;
	struct Library *				usb_TimerBase;
	struct TimeRequest *			usb_TimeRequest;

	struct InputIFace *				usb_IInput;
	struct Device *					usb_InputBase;
	struct IOStdReq *				usb_InputIORequest;

	// Master
	struct USB3_TaskNode			usb_Master_Task;
	PTR								usb_Master_Intern;
	struct USB3_MsgPort *			usb_Master_MsgPort;
	U32								usb_DriverDirLock;

	// Queued Memory -- MEMID_x Headers
	struct Mem_HeaderNode *			usb_Memory_Headers;

	// Any Task started
	struct USB3_Semaphore			usb_Task_Semaphore;
	struct USB3_Header				usb_Task_Header;

	// Any Driver started via Bind
	struct USB3_Semaphore			usb_Bind_Semaphore;
	struct USB3_Header				usb_Bind_Header;

	// Driver Classes/Types
	struct USB3_Semaphore			usb_FDriver_Semaphore;
	struct USB3_Header				usb_FDriver_Header;

	// Function Address
	struct USB3_Semaphore			usb_Addr_Semaphore;

	// Function Devices Connected (From HCD and HUBs)
	struct USB3_Semaphore			usb_Fkt_Semaphore;
	struct USB3_Header				usb_Fkt_Header;


	// --
	#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_INFO ) || defined( DO_DEBUG ) || defined( DO_TASKNAME )
	struct UtilityIFace *			_dbg_IUility;
	struct ExecIFace *				_dbg_IExec;
	struct Task *					_dbg_Task;
	#endif
	// --
	#ifdef DO_PANIC
	void				VARARGS68K	( *_Panic )				( struct USBBase *, STR file, U32 line, STR fmt, ... );
	#endif
	// --
	#ifdef DO_ERROR
	void				VARARGS68K	( *_Error )				( struct USBBase *, STR fmt, ... );
	#endif
	// --
	#ifdef DO_INFO
	void				VARARGS68K	( *_Info )				( struct USBBase *, STR fmt, ... );
	#endif
	// --
	#ifdef DO_DEBUG
	void				VARARGS68K	( *_Debug )				( struct USBBase *, STR fmt, ... );
	#endif
	// --
	#ifdef DO_TASKNAME
	STR					VARARGS68K	( *_Task )				( struct USBBase * );
	#endif
	// --

	/*
	** Define Device Functions
	**
	** struct USB3_ConfigNode *		( *_Config_Alloc )		( struct USBBase *usbbase, struct RealFunctionNode *ftk, STR file );
	** enum FSTAT					( *_Config_Free )		( struct USBBase *usbbase, struct USB3_ConfigNode *cn, STR file );
	*/

	#define USB3_CREATEFUNC(ret_type, name, ...)	ret_type (*_##name)(__VA_ARGS__)
	#include "usb3_Protos.h"
	#undef USB3_CREATEFUNC
};

// --

// -- Config

#define CONFIG_PARSE(x)						usbbase->_Config_Parse(usbbase,x)

// -- USB Driver


// -- EndPoint


// -- EndPointRes


// -- Function

#define FUNCTION_INIT(x,y)					usbbase->_Function_Init(usbbase,x,y)
#define FUNCTION_BIND(x,y)					usbbase->_Function_Bind(usbbase,x,y)
#define FUNCTION_CLAIM(x,y)					usbbase->_Function_Claim(usbbase,x,y)
#define FUNCTION_DECLAIM(x,y)				usbbase->_Function_Declaim(usbbase,x,y)
#define FUNCTION_MARKDETACH(x)				usbbase->_Function_MarkDetach(usbbase,x)
//#define FUNCTION_FREEDETACH(x)				usbbase->_Function_FreeDetach(usbbase,x)

// -- Function Driver

#define FDRIVER_CREATETAGS(...)				usbbase->_FDriver_CreateTags(usbbase,__VA_ARGS__)
#define FDRIVER_CREATELIST(x)				usbbase->_FDriver_CreateList(usbbase,x)
#define FDRIVER_LOAD(x)						usbbase->_FDriver_Load(usbbase,x)

// -- HCD

#define HCD_CONTROLLERS_FIND()				usbbase->_HCD_Controllers_Find(usbbase)
#define HCD_CONTROLLERS_START()				usbbase->_HCD_Controllers_Start(usbbase)
#define HCD_CONTROLLERS_STOP()				usbbase->_HCD_Controllers_Stop(usbbase)
//#define HCD_SIGNAL_DRIVERNODE(x)			usbbase->_HCD_Signal_DriverNode(usbbase,x)
#define HCD_ADDR_OBTAIN(x,y)				usbbase->_HCD_Addr_Obtain(usbbase,x,y)
#define HCD_ADDR_RELEASE(x)					usbbase->_HCD_Addr_Release(usbbase,x)
#define HCD_ADD_REQUEST(x,y)				usbbase->_HCD_Add_Request(usbbase,x,y)
#define HCD_WAIT_MS(x,y)					usbbase->_HCD_Wait_ms(usbbase,x,y)
#define HCD_RESTART_ENDPOINT(x,y)			usbbase->_HCD_Restart_EndPoint(usbbase,x,y)
#define HCD_TRANSFER_CHECK(x,y,z)			usbbase->_HCD_Transfer_Check(usbbase,x,y,z)
#define HCD_TRANSFER_REMOVE(x,y)			usbbase->_HCD_Transfer_Remove(usbbase,x,y)

// -- Interface

#define INTERFACE_CLAIMHEADER(x,y)			usbbase->_Interface_ClaimHeader(usbbase,x,y)
#define INTERFACE_DECLAIMHEADER(x,y)		usbbase->_Interface_DeclaimHeader(usbbase,x,y)


// -- IORequest


#define IOREQUEST_ALLOCTAGS(...)			usbbase->_IORequest_AllocTags(usbbase,__VA_ARGS__)

// -- Master

#define MASTER_STARTUP()					usbbase->_Master_Startup(usbbase)
#define MASTER_SHUTDOWN()					usbbase->_Master_Shutdown(usbbase)

// -- Memory

#define MEMORY_SETUP()						usbbase->_Memory_Setup(usbbase)
#define MEMORY_CLEANUP()					usbbase->_Memory_Cleanup(usbbase)

// -- Process Message

// -- Register

#define REGISTER_REGISTERTAGS(...)			usbbase->_Register_RegisterTags(usbbase,__VA_ARGS__)
#define REGISTER_REGISTERLIST(x)			usbbase->_Register_RegisterList(usbbase,x)

// -- RootHub

#define ROOTHUB_HANDLE_READ(x,y)			usbbase->_RootHUB_Handle_Read(usbbase,x,y)
#define ROOTHUB_HANDLE_WRITE(x,y)			usbbase->_RootHUB_Handle_Write(usbbase,x,y)
#define ROOTHUB_HANDLE_CHANGE(x)			usbbase->_RootHUB_Handle_Chg(usbbase,x)

// -- Misc

#define ASYNC_INIT(x,y)						usbbase->_ASync_Init(usbbase,x,y)
#define ASYNC_FREE(x)						usbbase->_ASync_Free(usbbase,x)
#define ASYNC_ADD(x)						usbbase->_ASync_Add(usbbase,x)
#define ASYNC_SUB(x)						usbbase->_ASync_Sub(usbbase,x)
#define ASYNC_WAIT(x)						usbbase->_ASync_Wait(usbbase,x)

#define DESC_NEXT_DESC(x)					usbbase->_Desc_Next_Desc(usbbase,x)
#define DESC_NEXT_INTERFACE(x,y)			usbbase->_Desc_Next_Interface(usbbase,x,y)

#define MEM_SET(x,y,z)						usbbase->_Mem_Set(usbbase,x,y,z)
#define MEM_COPY(x,y,z)						usbbase->_Mem_Copy(usbbase,x,y,z)
#define MEM_ALLOCVEC(x,y)					usbbase->_Mem_AllocVec(usbbase,x,y)
#define MEM_FREEVEC(x)						usbbase->_Mem_FreeVec(usbbase,x)
#define MEM_ALLOCIOBUFFER(x,y)				usbbase->_Mem_AllocIOBuffer(usbbase,x,y)
#define MEM_FREEIOBUFFER(x)					usbbase->_Mem_FreeIOBuffer(usbbase,x)
#define MEM_PRINTF(x,...)					usbbase->_Mem_PrintF(usbbase,x,__VA_ARGS__)

#define MISC_NEWNOTIFYID()					usbbase->_Misc_NewNotifyID(usbbase)
#define MISC_NEXTTAGITEM(x)					usbbase->_Misc_NextTagItem(usbbase,x)
#define MISC_GETTAGVALUE(x,y,z)				usbbase->_Misc_GetTagValue(usbbase,x,y,z)
#define MISC_GETTAGDATA(x,y,z)				usbbase->_Misc_GetTagData(usbbase,x,y,z)
#define MISC_WAIT(x)						usbbase->_Misc_Wait(usbbase,x)
#define MISC_MICRODELAY(x)					usbbase->_Misc_MicroDelay(usbbase,x)
#define MISC_ADDINTSERVER(x,y)				usbbase->_Misc_AddIntServer(usbbase,x,y)
#define MISC_REMINTSERVER(x,y)				usbbase->_Misc_RemIntServer(usbbase,x,y)
#define MISC_ADDRESETCALLBACK(x)			usbbase->_Misc_AddResetCallback(usbbase,x)
#define MISC_REMRESETCALLBACK(x)			usbbase->_Misc_RemResetCallback(usbbase,x)
#define MISC_COLDREBOOT()					usbbase->_Misc_ColdReboot(usbbase)
#define MISC_ICECOLDREBOOT()				usbbase->_Misc_IceColdReboot(usbbase)
#define MISC_GETINTUIPREFS(x,y)				usbbase->_Misc_GetIntuiPrefs(usbbase,x,y)
#define MISC_SETCURRENTDIR(x)				usbbase->_Misc_SetCurrentDir(usbbase,x)
#define MISC_SETPROCWINDOW(x)				usbbase->_Misc_SetProcWindow(usbbase,x)
#define MISC_STRCASECOMPARE(x,y)			usbbase->_Misc_StrCaseCompare(usbbase,x,y)
#define MISC_STRICASECOMPARE(x,y)			usbbase->_Misc_StrICaseCompare(usbbase,x,y)
#define MISC_STRINCASECOMPARE(x,y,z)		usbbase->_Misc_StrINCaseCompare(usbbase,x,y,z)
#define MISC_STRLENGTH(x)					usbbase->_Misc_StrLength(usbbase,x)
#define MISC_OPENFILE(x,y)					usbbase->_Misc_OpenFile(usbbase,x,y)
#define MISC_CLOSEFILE(x)					usbbase->_Misc_CloseFile(usbbase,x)
#define MISC_GETFILESIZE(x)					usbbase->_Misc_GetFileSize(usbbase,x)
#define MISC_LOADFILE(x,y)					usbbase->_Misc_LoadFile(usbbase,x,y)
#define MISC_READFILE(x,y,z)				usbbase->_Misc_ReadFile(usbbase,x,y,z)
#define MISC_OPENLIBRARY(x,y)				usbbase->_Misc_OpenLibrary(usbbase,x,y)
#define MISC_CLOSELIBRARY(x)				usbbase->_Misc_CloseLibrary(usbbase,x)
#define MISC_OBTAININTERFACE(x,y,z)			usbbase->_Misc_ObtainInterface(usbbase,x,y,z)
#define MISC_RELEASEINTERFACE(x)			usbbase->_Misc_ReleaseInterface(usbbase,x)
#define MISC_ADDDEVICE(x)					usbbase->_Misc_AddDevice(usbbase,x)
#define MISC_CREATELIBRARY(x)				usbbase->_Misc_CreateLibrary(usbbase,x)

#define SETUPDATA_ALLOC()					usbbase->_SetupData_Alloc(usbbase)
#define SETUPDATA_FREE(x)					usbbase->_SetupData_Free(usbbase,x)

#define NODE_ADDHEAD(x,y)					usbbase->_Node_AddHead(usbbase,x,y)
#define NODE_ADDTAIL(x,y)					usbbase->_Node_AddTail(usbbase,x,y)
#define NODE_ADDINFRONTOF(x,y,z)			usbbase->_Node_AddInfrontOf(usbbase,x,y,z)
#define NODE_REMHEAD(x)						usbbase->_Node_RemHead(usbbase,x)
#define NODE_REMNODE(x,y)					usbbase->_Node_RemNode(usbbase,x,y)

#define SEMAPHORE_INIT(x)					usbbase->_Semaphore_Init(usbbase,x)
#define SEMAPHORE_FREE(x)					usbbase->_Semaphore_Free(usbbase,x)
#define SEMAPHORE_OBTAIN(x)					usbbase->_Semaphore_Obtain(usbbase,x)
#define SEMAPHORE_RELEASE(x)				usbbase->_Semaphore_Release(usbbase,x)
#define SEMAPHORE_ATTEMPT(x)				usbbase->_Semaphore_Attempt(usbbase,x)

//#define TASK_RUN(x,y)						usbbase->_Task_Run(usbbase,x,y)
#define TASK_FIND()							usbbase->_Task_Find(usbbase)
#define TASK_WAIT(x)						usbbase->_Task_Wait(usbbase,x)
#define TASK_SIGNAL(x,y)					usbbase->_Task_Signal(usbbase,x,y)
#define TASK_SETSIGNAL(x,y)					usbbase->_Task_SetSignal(usbbase,x,y)
#define TASK_SETTLE(x)						usbbase->_Task_Settle(usbbase,x)
#define TASK_SETPRI(x)						usbbase->_Task_SetPri(usbbase,x)

// -- Task

#define TASK_START(...)						usbbase->_Task_Start(usbbase,__VA_ARGS__)

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

#define CONFIG_ALLOC(x)						usbbase->_Config_Alloc(usbbase,x,__FILE__)
#define CONFIG_FREE(x)						usbbase->_Config_Free(usbbase,x,__FILE__)
#define CONFIG_LOCK(x)						usbbase->_Config_Lock(usbbase,x,__FILE__)
#define CONFIG_UNLOCK(x)					usbbase->_Config_Unlock(usbbase,x,__FILE__)
#define CONFIG_VALID(x)						usbbase->_Config_Valid(usbbase,x,__FILE__)
#define CONFIG_SET(x,y,z)					usbbase->_Config_Set(usbbase,x,y,z,__FILE__)
#define DRIVER_ALLOC(x,y)					usbbase->_Driver_Alloc(usbbase,x,y,__FILE__)
#define DRIVER_FREE(x)						usbbase->_Driver_Free(usbbase,x,__FILE__)
#define DRIVER_LOCK(x)						usbbase->_Driver_Lock(usbbase,x,__FILE__)
#define DRIVER_UNLOCK(x)					usbbase->_Driver_Unlock(usbbase,x,__FILE__)
#define DRIVER_VALID(x)						usbbase->_Driver_Valid(usbbase,x,__FILE__)
#define ENDPOINT_ALLOC(x,y,z)				usbbase->_EndPoint_Alloc(usbbase,x,y,z,__FILE__)
#define ENDPOINT_FREE(x)					usbbase->_EndPoint_Free(usbbase,x,__FILE__)
#define ENDPOINT_LOCK(x)					usbbase->_EndPoint_Lock(usbbase,x,__FILE__)
#define ENDPOINT_UNLOCK(x)					usbbase->_EndPoint_Unlock(usbbase,x,__FILE__)
#define ENDPOINT_VALID(x)					usbbase->_EndPoint_Valid(usbbase,x,__FILE__)
#define ENDPOINTRES_ALLOC()					usbbase->_EndPointRes_Alloc(usbbase,__FILE__)
#define ENDPOINTRES_FREE(x)					usbbase->_EndPointRes_Free(usbbase,x,__FILE__)
#define ENDPOINTRES_LOCK(x)					usbbase->_EndPointRes_Lock(usbbase,x,__FILE__)
#define ENDPOINTRES_UNLOCK(x)				usbbase->_EndPointRes_Unlock(usbbase,x,__FILE__)
#define ENDPOINTRES_VALID(x)				usbbase->_EndPointRes_Valid(usbbase,x,__FILE__)
#define ENDPOINTRES_OBTAINLIST(x,y)			usbbase->_EndPointRes_ObtainList(usbbase,__FILE__,x,y)
#define ENDPOINTRES_RELEASE(x)				usbbase->_EndPointRes_Release(usbbase,x,__FILE__)
#define ENDPOINTRES_DESTALL(x)				usbbase->_EndPointRes_Destall(usbbase,x,__FILE__)
#define FDRIVER_ALLOC()						usbbase->_FDriver_Alloc(usbbase,__FILE__)
#define FDRIVER_FREE(x)						usbbase->_FDriver_Free(usbbase,x,__FILE__)
#define FDRIVER_LOCK(x)						usbbase->_FDriver_Lock(usbbase,x,__FILE__)
#define FDRIVER_UNLOCK(x)					usbbase->_FDriver_Unlock(usbbase,x,__FILE__)
#define FDRIVER_VALID(x)					usbbase->_FDriver_Valid(usbbase,x,__FILE__)
#define FUNCTION_ALLOC(x,y,z,q)				usbbase->_Function_Alloc(usbbase,x,y,z,q,__FILE__)
#define FUNCTION_FREE(x)					usbbase->_Function_Free(usbbase,x,__FILE__)
#define FUNCTION_LOCK(x)					usbbase->_Function_Lock(usbbase,x,__FILE__)
#define FUNCTION_UNLOCK(x)					usbbase->_Function_Unlock(usbbase,x,__FILE__)
#define FUNCTION_VALID(x)					usbbase->_Function_Valid(usbbase,x,__FILE__)
#define HCD_ALLOC(x)						usbbase->_HCD_Alloc(usbbase,x,__FILE__)
#define HCD_FREE(x)							usbbase->_HCD_Free(usbbase,x,__FILE__)
#define HCD_LOCK(x)							usbbase->_HCD_Lock(usbbase,x,__FILE__)
#define HCD_UNLOCK(x)						usbbase->_HCD_Unlock(usbbase,x,__FILE__)
#define HCD_VALID(x)						usbbase->_HCD_Valid(usbbase,x,__FILE__)
#define HCD_REPLY(x)						usbbase->_HCD_Reply(usbbase,x,__FILE__)
#define INTERFACE_ALLOCNODE(x)				usbbase->_Interface_AllocNode(usbbase,x,__FILE__)
#define INTERFACE_FREENODE(x)				usbbase->_Interface_FreeNode(usbbase,x,__FILE__)
#define INTERFACE_LOCKNODE(x)				usbbase->_Interface_LockNode(usbbase,x,__FILE__)
#define INTERFACE_UNLOCKNODE(x)				usbbase->_Interface_UnlockNode(usbbase,x,__FILE__)
#define INTERFACE_VALIDNODE(x)				usbbase->_Interface_ValidNode(usbbase,x,__FILE__)
#define INTERFACE_ALLOCHEADER(x,y)			usbbase->_Interface_AllocHeader(usbbase,x,y,__FILE__)
#define INTERFACE_FREEHEADER(x)				usbbase->_Interface_FreeHeader(usbbase,x,__FILE__)
#define INTERFACE_LOCKHEADER(x)				usbbase->_Interface_LockHeader(usbbase,x,__FILE__)
#define INTERFACE_UNLOCKHEADER(x)			usbbase->_Interface_UnlockHeader(usbbase,x,__FILE__)
#define INTERFACE_VALIDHEADER(x)			usbbase->_Interface_ValidHeader(usbbase,x,__FILE__)
#define INTERFACE_ALLOCGROUP(x)				usbbase->_Interface_AllocGroup(usbbase,x,__FILE__)
#define INTERFACE_FREEGROUP(x)				usbbase->_Interface_FreeGroup(usbbase,x,__FILE__)
#define INTERFACE_LOCKGROUP(x)				usbbase->_Interface_LockGroup(usbbase,x,__FILE__)
#define INTERFACE_UNLOCKGROUP(x)			usbbase->_Interface_UnlockGroup(usbbase,x,__FILE__)
#define INTERFACE_VALIDGROUP(x)				usbbase->_Interface_ValidGroup(usbbase,x,__FILE__)
#define IOREQUEST_ALLOCLIST(x)				usbbase->_IORequest_AllocList(usbbase,x,__FILE__)
#define IOREQUEST_FREE(x)					usbbase->_IORequest_Free(usbbase,x,__FILE__)
#define IOREQUEST_LOCK(x)					usbbase->_IORequest_Lock(usbbase,x,__FILE__)
#define IOREQUEST_UNLOCK(x)					usbbase->_IORequest_Unlock(usbbase,x,__FILE__)
#define IOREQUEST_VALID(x)					usbbase->_IORequest_Valid(usbbase,x,__FILE__)
#define MEMORY_ALLOC(x,y)					usbbase->_Memory_Alloc(usbbase,x,y,__FILE__)
#define MEMORY_FREE(x,y,z)					usbbase->_Memory_Free(usbbase,x,y,z,__FILE__)
#define MISC_DOCOMMAND(x)					usbbase->_Misc_DoCommand(usbbase,x,__FILE__)
#define MSGPORT_INIT(x)						usbbase->_MsgPort_Init(usbbase,x,__FILE__)
#define MSGPORT_FREE(x)						usbbase->_MsgPort_Free(usbbase,x,__FILE__)
#define MSGPORT_GETMSG(x)					usbbase->_MsgPort_GetMsg(usbbase,x,__FILE__)
#define MSGPORT_PUTMSG(x,y)					usbbase->_MsgPort_PutMsg(usbbase,x,y,__FILE__)
#define MSGPORT_REPLYMSG(x)					usbbase->_MsgPort_ReplyMsg(usbbase,x,__FILE__)
#define MSGPORT_REINIT(x)					usbbase->_MsgPort_Reinit(usbbase,x,__FILE__)
#define TASK_ALLOC(x)						usbbase->_Task_Alloc(usbbase,x,__FILE__)
#define TASK_FREE(x)						usbbase->_Task_Free(usbbase,x,__FILE__)
#define TASK_LOCK(x)						usbbase->_Task_Lock(usbbase,x,__FILE__)
#define TASK_UNLOCK(x)						usbbase->_Task_Unlock(usbbase,x,__FILE__)
#define TASK_VALID(x)						usbbase->_Task_Valid(usbbase,x,__FILE__)
#define TASK_STOP_TN(x)						usbbase->_Task_Stop_TN(usbbase,x,__FILE__)
#define TASK_STOP_DN(x)						usbbase->_Task_Stop_DN(usbbase,x,__FILE__)
#define TASK_STOP_FN(x)						usbbase->_Task_Stop_FN(usbbase,x,__FILE__)
#define TASK_ALLOCSIGNAL(x)					usbbase->_Task_AllocSignal(usbbase,x,__FILE__)
#define TASK_FREESIGNAL(x)					usbbase->_Task_FreeSignal(usbbase,x,__FILE__)
#define TASK_REALLOCSIGNAL(x)				usbbase->_Task_ReallocSignal(usbbase,x,__FILE__)
#define TASKMSG_ALLOC()						usbbase->_TaskMsg_Alloc(usbbase,__FILE__)
#define TASKMSG_FREE(x)						usbbase->_TaskMsg_Free(usbbase,x,__FILE__)
#define TASKMSG_LOCK(x)						usbbase->_TaskMsg_Lock(usbbase,x,__FILE__)
#define TASKMSG_UNLOCK(x)					usbbase->_TaskMsg_Unlock(usbbase,x,__FILE__)
#define TASKMSG_VALID(x)					usbbase->_TaskMsg_Valid(usbbase,x,__FILE__)
#define REGISTER_ALLOC()					usbbase->_Register_Alloc(usbbase,__FILE__)
#define REGISTER_FREE(x)					usbbase->_Register_Free(usbbase,x,__FILE__)
#define REGISTER_LOCK(x)					usbbase->_Register_Lock(usbbase,x,__FILE__)
#define REGISTER_UNLOCK(x)					usbbase->_Register_Unlock(usbbase,x,__FILE__)
#define REGISTER_UNREGISTER(x)				usbbase->_Register_Free(usbbase,x,__FILE__)
#define REGISTER_VALID(x)					usbbase->_Register_Valid(usbbase,x,__FILE__)

#define ENDPOINTRES_OBTAINTAGS(x,...)		usbbase->_EndPointRes_ObtainTags(usbbase,__FILE__,x,__VA_ARGS__)

#define IO_DO(x)							usbbase->_IO_Do(usbbase,x,__FILE__)
#define IO_STOP(x)							usbbase->_IO_Stop(usbbase,x,__FILE__)
#define IO_SEND(x)							usbbase->_IO_Send(usbbase,x,__FILE__)
#define IO_ABORT(x)							usbbase->_IO_Abort(usbbase,x,__FILE__)
#define IO_RESTART(x)						usbbase->_IO_Restart(usbbase,x,__FILE__)
#define IOREQUEST_ACTIVE_ADD(x)				usbbase->_IORequest_Active_Add(usbbase,x,__FILE__)
#define IOREQUEST_ACTIVE_SUB(x)				usbbase->_IORequest_Active_Sub(usbbase,x,__FILE__)

#ifdef DO_DEBUG
#define TASKNODE_PRINT(x)					usbbase->_TaskNode_Print(usbbase,x)
#else
#define TASKNODE_PRINT(x)
#endif

#else

#define CONFIG_ALLOC(x)						usbbase->_Config_Alloc(usbbase,x)
#define CONFIG_FREE(x)						usbbase->_Config_Free(usbbase,x)
#define CONFIG_LOCK(x)						usbbase->_Config_Lock(usbbase,x)
#define CONFIG_UNLOCK(x)					usbbase->_Config_Unlock(usbbase,x)
#define CONFIG_VALID(x)						usbbase->_Config_Valid(usbbase,x)
#define CONFIG_SET(x,y,z)					usbbase->_Config_Set(usbbase,x,y,z)
#define DRIVER_ALLOC(x,y)					usbbase->_Driver_Alloc(usbbase,x,y)
#define DRIVER_FREE(x)						usbbase->_Driver_Free(usbbase,x)
#define DRIVER_LOCK(x)						usbbase->_Driver_Lock(usbbase,x)
#define DRIVER_UNLOCK(x)					usbbase->_Driver_Unlock(usbbase,x)
#define DRIVER_VALID(x)						usbbase->_Driver_Valid(usbbase,x)
#define ENDPOINT_ALLOC(x,y,z)				usbbase->_EndPoint_Alloc(usbbase,x,y,z)
#define ENDPOINT_FREE(x)					usbbase->_EndPoint_Free(usbbase,x)
#define ENDPOINT_LOCK(x)					usbbase->_EndPoint_Lock(usbbase,x)
#define ENDPOINT_UNLOCK(x)					usbbase->_EndPoint_Unlock(usbbase,x)
#define ENDPOINT_VALID(x)					usbbase->_EndPoint_Valid(usbbase,x)
#define ENDPOINTRES_ALLOC()					usbbase->_EndPointRes_Alloc(usbbase)
#define ENDPOINTRES_FREE(x)					usbbase->_EndPointRes_Free(usbbase,x)
#define ENDPOINTRES_LOCK(x)					usbbase->_EndPointRes_Lock(usbbase,x)
#define ENDPOINTRES_UNLOCK(x)				usbbase->_EndPointRes_Unlock(usbbase,x)
#define ENDPOINTRES_VALID(x)				usbbase->_EndPointRes_Valid(usbbase,x)
#define ENDPOINTRES_OBTAINLIST(x,y)			usbbase->_EndPointRes_ObtainList(usbbase,x,y)
#define ENDPOINTRES_RELEASE(x)				usbbase->_EndPointRes_Release(usbbase,x)
#define ENDPOINTRES_DESTALL(x)				usbbase->_EndPointRes_Destall(usbbase,x)
#define FDRIVER_ALLOC()						usbbase->_FDriver_Alloc(usbbase)
#define FDRIVER_FREE(x)						usbbase->_FDriver_Free(usbbase,x)
#define FDRIVER_LOCK(x)						usbbase->_FDriver_Lock(usbbase,x)
#define FDRIVER_UNLOCK(x)					usbbase->_FDriver_Unlock(usbbase,x)
#define FDRIVER_VALID(x)					usbbase->_FDriver_Valid(usbbase,x)
#define FUNCTION_ALLOC(x,y,z,q)				usbbase->_Function_Alloc(usbbase,x,y,z,q)
#define FUNCTION_FREE(x)					usbbase->_Function_Free(usbbase,x)
#define FUNCTION_LOCK(x)					usbbase->_Function_Lock(usbbase,x)
#define FUNCTION_UNLOCK(x)					usbbase->_Function_Unlock(usbbase,x)
#define FUNCTION_VALID(x)					usbbase->_Function_Valid(usbbase,x)
#define HCD_ALLOC(x)						usbbase->_HCD_Alloc(usbbase,x)
#define HCD_FREE(x)							usbbase->_HCD_Free(usbbase,x)
#define HCD_LOCK(x)							usbbase->_HCD_Lock(usbbase,x)
#define HCD_UNLOCK(x)						usbbase->_HCD_Unlock(usbbase,x)
#define HCD_VALID(x)						usbbase->_HCD_Valid(usbbase,x)
#define HCD_REPLY(x)						usbbase->_HCD_Reply(usbbase,x)
#define INTERFACE_ALLOCNODE(x)				usbbase->_Interface_AllocNode(usbbase,x)
#define INTERFACE_FREENODE(x)				usbbase->_Interface_FreeNode(usbbase,x)
#define INTERFACE_LOCKNODE(x)				usbbase->_Interface_LockNode(usbbase,x)
#define INTERFACE_UNLOCKNODE(x)				usbbase->_Interface_UnlockNode(usbbase,x)
#define INTERFACE_VALIDNODE(x)				usbbase->_Interface_ValidNode(usbbase,x)
#define INTERFACE_ALLOCHEADER(x,y)			usbbase->_Interface_AllocHeader(usbbase,x,y)
#define INTERFACE_FREEHEADER(x)				usbbase->_Interface_FreeHeader(usbbase,x)
#define INTERFACE_LOCKHEADER(x)				usbbase->_Interface_LockHeader(usbbase,x)
#define INTERFACE_UNLOCKHEADER(x)			usbbase->_Interface_UnlockHeader(usbbase,x)
#define INTERFACE_VALIDHEADER(x)			usbbase->_Interface_ValidHeader(usbbase,x)
#define INTERFACE_ALLOCGROUP(x)				usbbase->_Interface_AllocGroup(usbbase,x)
#define INTERFACE_FREEGROUP(x)				usbbase->_Interface_FreeGroup(usbbase,x)
#define INTERFACE_LOCKGROUP(x)				usbbase->_Interface_LockGroup(usbbase,x)
#define INTERFACE_UNLOCKGROUP(x)			usbbase->_Interface_UnlockGroup(usbbase,x)
#define INTERFACE_VALIDGROUP(x)				usbbase->_Interface_ValidGroup(usbbase,x)
#define IOREQUEST_ALLOCLIST(x)				usbbase->_IORequest_AllocList(usbbase,x)
#define IOREQUEST_FREE(x)					usbbase->_IORequest_Free(usbbase,x)
#define IOREQUEST_LOCK(x)					usbbase->_IORequest_Lock(usbbase,x)
#define IOREQUEST_UNLOCK(x)					usbbase->_IORequest_Unlock(usbbase,x)
#define IOREQUEST_VALID(x)					usbbase->_IORequest_Valid(usbbase,x)
#define MEMORY_ALLOC(x,y)					usbbase->_Memory_Alloc(usbbase,x,y)
#define MEMORY_FREE(x,y,z)					usbbase->_Memory_Free(usbbase,x,y,z)
#define MISC_DOCOMMAND(x)					usbbase->_Misc_DoCommand(usbbase,x)
#define MSGPORT_INIT(x)						usbbase->_MsgPort_Init(usbbase,x)
#define MSGPORT_FREE(x)						usbbase->_MsgPort_Free(usbbase,x)
#define MSGPORT_GETMSG(x)					usbbase->_MsgPort_GetMsg(usbbase,x)
#define MSGPORT_PUTMSG(x,y)					usbbase->_MsgPort_PutMsg(usbbase,x,y)
#define MSGPORT_REPLYMSG(x)					usbbase->_MsgPort_ReplyMsg(usbbase,x)
#define MSGPORT_REINIT(x)					usbbase->_MsgPort_Reinit(usbbase,x)
#define TASK_ALLOC(x)						usbbase->_Task_Alloc(usbbase,x)
#define TASK_FREE(x)						usbbase->_Task_Free(usbbase,x)
#define TASK_LOCK(x)						usbbase->_Task_Lock(usbbase,x)
#define TASK_UNLOCK(x)						usbbase->_Task_Unlock(usbbase,x)
#define TASK_VALID(x)						usbbase->_Task_Valid(usbbase,x)
#define TASK_STOP_TN(x)						usbbase->_Task_Stop_TN(usbbase,x)
#define TASK_STOP_DN(x)						usbbase->_Task_Stop_DN(usbbase,x)
#define TASK_STOP_FN(x)						usbbase->_Task_Stop_FN(usbbase,x)
#define TASK_ALLOCSIGNAL(x)					usbbase->_Task_AllocSignal(usbbase,x)
#define TASK_FREESIGNAL(x)					usbbase->_Task_FreeSignal(usbbase,x)
#define TASK_REALLOCSIGNAL(x)				usbbase->_Task_ReallocSignal(usbbase,x)
#define TASKMSG_ALLOC()						usbbase->_TaskMsg_Alloc(usbbase)
#define TASKMSG_FREE(x)						usbbase->_TaskMsg_Free(usbbase,x)
#define TASKMSG_LOCK(x)						usbbase->_TaskMsg_Lock(usbbase,x)
#define TASKMSG_UNLOCK(x)					usbbase->_TaskMsg_Unlock(usbbase,x)
#define TASKMSG_VALID(x)					usbbase->_TaskMsg_Valid(usbbase,x)
#define REGISTER_ALLOC()					usbbase->_Register_Alloc(usbbase)
#define REGISTER_FREE(x)					usbbase->_Register_Free(usbbase,x)
#define REGISTER_LOCK(x)					usbbase->_Register_Lock(usbbase,x)
#define REGISTER_UNLOCK(x)					usbbase->_Register_Unlock(usbbase,x)
#define REGISTER_UNREGISTER(x)				usbbase->_Register_Free(usbbase,x)
#define REGISTER_VALID(x)					usbbase->_Register_Valid(usbbase,x)

#define ENDPOINTRES_OBTAINTAGS(x,...)		usbbase->_EndPointRes_ObtainTags(usbbase,x,__VA_ARGS__)

#define IO_DO(x)							usbbase->_IO_Do(usbbase,x)
#define IO_STOP(x)							usbbase->_IO_Stop(usbbase,x)
#define IO_SEND(x)							usbbase->_IO_Send(usbbase,x)
#define IO_ABORT(x)							usbbase->_IO_Abort(usbbase,x)
#define IO_RESTART(x)						usbbase->_IO_Restart(usbbase,x)
#define IOREQUEST_ACTIVE_ADD(x)				usbbase->_IORequest_Active_Add(usbbase,x)
#define IOREQUEST_ACTIVE_SUB(x)				usbbase->_IORequest_Active_Sub(usbbase,x)

#define TASKNODE_PRINT(x)

#endif

// --

/***************************************************************************/

struct AbortIOMessage
{
	struct Message				msg;
	struct RealRequest *		ioreq;
	struct USB3_HCDNode *		HCD;
	struct RealFunctionNode *	Function;
};

/**************************************************************************/

#endif // INC_PRIVATE_DEVICE_H
