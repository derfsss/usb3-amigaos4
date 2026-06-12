
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/


/*
** -- Functions with only one proto
*/

// -- ASync

USB2_CREATEFUNC( S32,							ASync_Init,					struct USBBase *usbbase, struct USB2_ASync *ua, struct Task *owner );
USB2_CREATEFUNC( void,							ASync_Free,					struct USBBase *usbbase, struct USB2_ASync *ua );
USB2_CREATEFUNC( void,							ASync_Add,					struct USBBase *usbbase, struct USB2_ASync *ua );
USB2_CREATEFUNC( void,							ASync_Sub,					struct USBBase *usbbase, struct USB2_ASync *ua );
USB2_CREATEFUNC( S32,							ASync_Wait,					struct USBBase *usbbase, struct USB2_ASync *ua );

// -- Config

USB2_CREATEFUNC( S32,							Config_Parse,				struct USBBase *usbbase, struct RealFunctionNode *fkt );

// -- Descriptor

USB2_CREATEFUNC( struct USB2_Descriptor *,		Desc_Next_Desc,				struct USBBase *usbbase, struct USB2_Descriptor *desc );
USB2_CREATEFUNC( struct USB2_Interface_Desc *,	Desc_Next_Interface,		struct USBBase *usbbase, struct RealRegister *reg, struct USB2_Interface_Desc *ifcdsc );

// -- Function

USB2_CREATEFUNC( S32,							Function_Init,				struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as );
USB2_CREATEFUNC( S32,							Function_Bind,				struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as );
USB2_CREATEFUNC( S32,							Function_Claim,				struct USBBase *usbbase, struct RealRegister *reg, struct RealFunctionNode *fn );
USB2_CREATEFUNC( void,							Function_Declaim,			struct USBBase *usbbase, struct RealRegister *reg, struct RealFunctionNode *fn );
USB2_CREATEFUNC( void,							Function_MarkDetach,		struct USBBase *usbbase, struct RealFunctionNode *fn );
//USB2_CREATEFUNC( void,						Function_FreeDetach,		struct USBBase *usbbase, struct RealFunctionNode *fn );

// -- Function Driver

USB2_CREATEFUNC( struct USB2_FktDriverNode * VARARGS68K, FDriver_CreateTags, struct USBBase *usbbase, ... );
USB2_CREATEFUNC( struct USB2_FktDriverNode *,	FDriver_CreateList,			struct USBBase *usbbase, struct TagItem *taglist );
USB2_CREATEFUNC( struct USB2_FktDriverNode *,	FDriver_Load,				struct USBBase *usbbase, STR filename );

// -- HCD

USB2_CREATEFUNC( U32,							HCD_Controllers_Find,		struct USBBase *usbbase );
USB2_CREATEFUNC( S32,							HCD_Controllers_Start,		struct USBBase *usbbase );
USB2_CREATEFUNC( void,							HCD_Controllers_Stop,		struct USBBase *usbbase );
USB2_CREATEFUNC( U32,							HCD_Addr_Obtain,			struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealFunctionNode *fn );
USB2_CREATEFUNC( void,							HCD_Addr_Release,			struct USBBase *usbbase, struct RealFunctionNode *fn );
USB2_CREATEFUNC( S32,							HCD_Add_Request,			struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq );
USB2_CREATEFUNC( void,							HCD_Wait_ms,				struct USBBase *usbbase, struct USB2_HCDNode *hn, U32 ms );
USB2_CREATEFUNC( S32,							HCD_Transfer_Check,			struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq, U32 force );
USB2_CREATEFUNC( void,							HCD_Restart_EndPoint,		struct USBBase *usbbase, struct USB2_HCDNode *hn, struct USB2_EndPointNode *ep );
USB2_CREATEFUNC( void,							HCD_Transfer_Remove,		struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq );

// -- Interface

USB2_CREATEFUNC( S32,							Interface_ClaimHeader,		struct USBBase *usbbase, struct RealRegister *reg, struct USB2_InterfaceHeader *ih );
USB2_CREATEFUNC( void,							Interface_DeclaimHeader,	struct USBBase *usbbase, struct RealRegister *reg, struct USB2_InterfaceHeader *ih );

// -- IORequest

USB2_CREATEFUNC( struct RealRequest * VARARGS68K, IORequest_AllocTags,		struct USBBase *usbbase, ... );

// -- Master

USB2_CREATEFUNC( S32,							Master_Startup,				struct USBBase *usbbase );
USB2_CREATEFUNC( void,							Master_Shutdown,			struct USBBase *usbbase );

// -- Memory

USB2_CREATEFUNC( void,							Mem_Set,					struct USBBase *usbbase, PTR mem, U32 value, U32 size );
USB2_CREATEFUNC( void,							Mem_Copy,					struct USBBase *usbbase, PTR src, PTR dst, U32 size );
USB2_CREATEFUNC( PTR,							Mem_AllocVec,				struct USBBase *usbbase, U32 size, S32 clr );
USB2_CREATEFUNC( void,							Mem_FreeVec,				struct USBBase *usbbase, PTR mem );
USB2_CREATEFUNC( PTR,							Mem_AllocIOBuffer,			struct USBBase *usbbase, U32 size, S32 clr );
USB2_CREATEFUNC( void,							Mem_FreeIOBuffer,			struct USBBase *usbbase, PTR mem );
USB2_CREATEFUNC( STR VARARGS68K,				Mem_PrintF,					struct USBBase *usbbase, STR fmt, ... );

// -- Node

USB2_CREATEFUNC( void,							Node_AddHead,				struct USBBase *usbbase, struct USB2_Header *header, PTR n );
USB2_CREATEFUNC( void,							Node_AddTail,				struct USBBase *usbbase, struct USB2_Header *header, PTR n );
USB2_CREATEFUNC( void,							Node_AddInfrontOf,			struct USBBase *usbbase, struct USB2_Header *header, PTR c, PTR n );
USB2_CREATEFUNC( PTR,							Node_RemHead,				struct USBBase *usbbase, struct USB2_Header *header );
USB2_CREATEFUNC( void,							Node_RemNode,				struct USBBase *usbbase, struct USB2_Header *header, PTR n );

// -- Register

USB2_CREATEFUNC( struct RealRegister * VARARGS68K, Register_RegisterTags,	struct USBBase *usbbase, ... );
USB2_CREATEFUNC( struct RealRegister *,			Register_RegisterList,		struct USBBase *usbbase, struct TagItem *taglist );

// -- Root HUB

USB2_CREATEFUNC( S32,							RootHUB_Handle_Read,		struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq );
USB2_CREATEFUNC( S32,							RootHUB_Handle_Write,		struct USBBase *usbbase, struct USB2_HCDNode *hn, struct RealRequest *ioreq );
USB2_CREATEFUNC( void,							RootHUB_Handle_Chg,			struct USBBase *usbbase, struct USB2_HCDNode *hn );

// -- Semaphore

USB2_CREATEFUNC( void,							Semaphore_Init,				struct USBBase *usbbase, struct USB2_Semaphore *us );
USB2_CREATEFUNC( void,							Semaphore_Free,				struct USBBase *usbbase, struct USB2_Semaphore *us );
USB2_CREATEFUNC( void,							Semaphore_Obtain,			struct USBBase *usbbase, struct USB2_Semaphore *us );
USB2_CREATEFUNC( void,							Semaphore_Release,			struct USBBase *usbbase, struct USB2_Semaphore *us );
USB2_CREATEFUNC( S32,							Semaphore_Attempt,			struct USBBase *usbbase, struct USB2_Semaphore *us );

// -- SetupData Buffer

USB2_CREATEFUNC( PTR,							SetupData_Alloc,			struct USBBase *usbbase );
USB2_CREATEFUNC( void, 							SetupData_Free,				struct USBBase *usbbase, PTR sd );

// -- Task

USB2_CREATEFUNC( enum TaskReturn,				Task_Start,					struct USBBase *usbbase, ... );
USB2_CREATEFUNC( PTR,							Task_Find,					struct USBBase *usbbase );
USB2_CREATEFUNC( U32,							Task_Wait,					struct USBBase *usbbase, U32 mask );
USB2_CREATEFUNC( void,							Task_Signal,				struct USBBase *usbbase, PTR task, U32 signals );
USB2_CREATEFUNC( U32,							Task_SetSignal,				struct USBBase *usbbase, U32 a, U32 b );
USB2_CREATEFUNC( void,							Task_Settle,				struct USBBase *usbbase, struct USB2_TaskNode *tn );
USB2_CREATEFUNC( S32,							Task_SetPri,				struct USBBase *usbbase, S32 pri );

// -- Memory

USB2_CREATEFUNC( S32,							Memory_Setup,				struct USBBase *usbbase );
USB2_CREATEFUNC( void,							Memory_Cleanup,				struct USBBase *usbbase );

// -- Misc

USB2_CREATEFUNC( USB2_ID,						Misc_NewNotifyID,			struct USBBase *usbbase );
USB2_CREATEFUNC( struct TagItem *,				Misc_NextTagItem,			struct USBBase *usbbase, struct TagItem **tag );
USB2_CREATEFUNC( U32,							Misc_GetTagValue,			struct USBBase *usbbase, U32 tagid, U32 defvalue, struct TagItem *taglist );
USB2_CREATEFUNC( PTR,							Misc_GetTagData,			struct USBBase *usbbase, U32 tagid, U32 defvalue, struct TagItem *taglist );
USB2_CREATEFUNC( void,							Misc_Wait,					struct USBBase *usbbase, U32 secs );
USB2_CREATEFUNC( void,							Misc_MicroDelay,			struct USBBase *usbbase, U32 ms );
USB2_CREATEFUNC( S32,							Misc_AddIntServer,			struct USBBase *usbbase, U32 nr, struct Interrupt *i );
USB2_CREATEFUNC( void,							Misc_RemIntServer,			struct USBBase *usbbase, U32 nr, struct Interrupt *i );
USB2_CREATEFUNC( int,							Misc_AddResetCallback,		struct USBBase *usbbase, struct Interrupt *i );
USB2_CREATEFUNC( void,							Misc_RemResetCallback,		struct USBBase *usbbase, struct Interrupt *i );
USB2_CREATEFUNC( void,							Misc_ColdReboot,			struct USBBase *usbbase );
USB2_CREATEFUNC( void,							Misc_IceColdReboot,			struct USBBase *usbbase );
USB2_CREATEFUNC( PTR,							Misc_GetIntuiPrefs,			struct USBBase *usbbase, PTR buf, U32 size );
USB2_CREATEFUNC( S32,							Misc_SetCurrentDir,			struct USBBase *usbbase, S32 new );
USB2_CREATEFUNC( S32,							Misc_SetProcWindow,			struct USBBase *usbbase, S32 val );
//USB2_CREATEFUNC( S32,							Misc_StrCaseCompare,		struct USBBase *usbbase, STR s1, STR s2 );
USB2_CREATEFUNC( S32,							Misc_StrICaseCompare,		struct USBBase *usbbase, STR s1, STR s2 );
USB2_CREATEFUNC( S32,							Misc_StrINCaseCompare,		struct USBBase *usbbase, STR s1, STR s2, S32 len );
USB2_CREATEFUNC( U32,							Misc_StrLength,				struct USBBase *usbbase, STR str );
USB2_CREATEFUNC( S32,							Misc_OpenFile,				struct USBBase *usbbase, STR filename, U32 filemode );
USB2_CREATEFUNC( void,							Misc_CloseFile,				struct USBBase *usbbase, S32 handle );
USB2_CREATEFUNC( S32,							Misc_GetFileSize,			struct USBBase *usbbase, S32 handle );
USB2_CREATEFUNC( STR,							Misc_LoadFile,				struct USBBase *usbbase, STR filename, U32 *filesize );
USB2_CREATEFUNC( S32,							Misc_ReadFile,				struct USBBase *usbbase, S32 handle, STR mem, S32 size );
USB2_CREATEFUNC( PTR,							Misc_OpenLibrary,			struct USBBase *usbbase, STR name, U32 ver );
USB2_CREATEFUNC( void,							Misc_CloseLibrary,			struct USBBase *usbbase, PTR base );
USB2_CREATEFUNC( PTR,							Misc_ObtainInterface,		struct USBBase *usbbase, PTR base, STR name, U32 ver );
USB2_CREATEFUNC( void,							Misc_ReleaseInterface,		struct USBBase *usbbase, PTR ifc );
USB2_CREATEFUNC( void,							Misc_AddDevice,				struct USBBase *usbbase, PTR base );
USB2_CREATEFUNC( PTR,							Misc_CreateLibrary,			struct USBBase *usbbase, PTR taglist );

/*
** -- Functions for Debug only
*/

#ifdef DO_DEBUG

USB2_CREATEFUNC( void,							TaskNode_Print,				struct USBBase *usbbase, struct USB2_TaskNode *tn );

#endif



/*
** -- Functions for Safety only
*/

#ifdef DO_PANIC


#endif







/*
** -- Functions with 2 types of Protos, depending on Defines
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

// -- Config

USB2_CREATEFUNC( struct USB2_ConfigNode *,		Config_Alloc,				struct USBBase *usbbase, struct RealFunctionNode *ftk, STR file );
USB2_CREATEFUNC( enum FSTAT,					Config_Free,				struct USBBase *usbbase, struct USB2_ConfigNode *cn, STR file );
USB2_CREATEFUNC( enum LSTAT,					Config_Lock,				struct USBBase *usbbase, struct USB2_ConfigNode *cn, STR file );
USB2_CREATEFUNC( void,							Config_Unlock,				struct USBBase *usbbase, struct USB2_ConfigNode *cn, STR file );
USB2_CREATEFUNC( enum VSTAT,					Config_Valid,				struct USBBase *usbbase, struct USB2_ConfigNode *cn, STR file );
USB2_CREATEFUNC( S32,							Config_Set,					struct USBBase *usbbase, struct RealFunctionNode *fkt, struct RealRequest *IOReq, U32 nr, STR file );

// -- Driver

USB2_CREATEFUNC( struct USB2_DriverNode *,		Driver_Alloc,				struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as, STR file );
USB2_CREATEFUNC( enum FSTAT,					Driver_Free,				struct USBBase *usbbase, struct USB2_DriverNode *dn, STR file );
USB2_CREATEFUNC( enum LSTAT,					Driver_Lock,				struct USBBase *usbbase, struct USB2_DriverNode *dn, STR file );
USB2_CREATEFUNC( void,							Driver_Unlock,				struct USBBase *usbbase, struct USB2_DriverNode *dn, STR file );
USB2_CREATEFUNC( enum VSTAT,					Driver_Valid,				struct USBBase *usbbase, struct USB2_DriverNode *dn, STR file );

// -- EndPoint

USB2_CREATEFUNC( struct USB2_EndPointNode *,	EndPoint_Alloc,				struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_InterfaceNode *in, struct USB2_EndPoint_Desc *epdsc, STR file );
USB2_CREATEFUNC( enum FSTAT,					EndPoint_Free,				struct USBBase *usbbase, struct USB2_EndPointNode *ep, STR file );
USB2_CREATEFUNC( enum LSTAT,					EndPoint_Lock,				struct USBBase *usbbase, struct USB2_EndPointNode *ep, STR file );
USB2_CREATEFUNC( void,							EndPoint_Unlock,			struct USBBase *usbbase, struct USB2_EndPointNode *ep, STR file );
USB2_CREATEFUNC( enum VSTAT,					EndPoint_Valid,				struct USBBase *usbbase, struct USB2_EndPointNode *ep, STR file );

// -- EndPoint Resource

USB2_CREATEFUNC( struct RealEndPointResource *,	EndPointRes_Alloc,			struct USBBase *usbbase, STR file );
USB2_CREATEFUNC( enum FSTAT,					EndPointRes_Free,			struct USBBase *usbbase, struct RealEndPointResource *epr, STR file );
USB2_CREATEFUNC( enum LSTAT,					EndPointRes_Lock,			struct USBBase *usbbase, struct RealEndPointResource *epr, STR file );
USB2_CREATEFUNC( void,							EndPointRes_Unlock,			struct USBBase *usbbase, struct RealEndPointResource *epr, STR file );
USB2_CREATEFUNC( enum VSTAT,					EndPointRes_Valid,			struct USBBase *usbbase, struct RealEndPointResource *epr, STR file );
USB2_CREATEFUNC( struct USB2_EPResource * VARARGS68K, EndPointRes_ObtainTags, struct USBBase *usbbase, STR file, struct RealRegister *reg, ... );
USB2_CREATEFUNC( struct USB2_EPResource *,		EndPointRes_ObtainList,		struct USBBase *usbbase, STR file, struct RealRegister *reg, struct TagItem *taglist );
USB2_CREATEFUNC( enum FSTAT,					EndPointRes_Release,		struct USBBase *usbbase, struct USB2_EPResource *epr, STR file );
USB2_CREATEFUNC( U32,							EndPointRes_Destall,		struct USBBase *usbbase, struct USB2_EPResource *epr, STR file );

// -- FDriver 

USB2_CREATEFUNC( struct USB2_FktDriverNode *,	FDriver_Alloc,				struct USBBase *usbbase, STR file );
USB2_CREATEFUNC( enum FSTAT,					FDriver_Free,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn, STR file );
USB2_CREATEFUNC( enum LSTAT,					FDriver_Lock,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn, STR file );
USB2_CREATEFUNC( void,							FDriver_Unlock,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn, STR file );
USB2_CREATEFUNC( enum VSTAT,					FDriver_Valid,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn, STR file );

// -- Function 

USB2_CREATEFUNC( struct RealFunctionNode *,		Function_Alloc,				struct USBBase *usbbase, struct USB2_HCDNode *hn, struct USB2_ASync *as, U32 Speed, U32 Tier, STR file );
USB2_CREATEFUNC( enum FSTAT,					Function_Free,				struct USBBase *usbbase, struct RealFunctionNode *fn, STR file );
USB2_CREATEFUNC( enum LSTAT,					Function_Lock,				struct USBBase *usbbase, struct RealFunctionNode *fn, STR file );
USB2_CREATEFUNC( void,							Function_Unlock,			struct USBBase *usbbase, struct RealFunctionNode *fn, STR file );
USB2_CREATEFUNC( enum VSTAT,					Function_Valid,				struct USBBase *usbbase, struct RealFunctionNode *fn, STR file );

// -- HCD

USB2_CREATEFUNC( struct USB2_HCDNode *,			HCD_Alloc,					struct USBBase *usbbase, U32 hcdtype, STR file );
USB2_CREATEFUNC( enum FSTAT,					HCD_Free,					struct USBBase *usbbase, struct USB2_HCDNode *hn, STR file );
USB2_CREATEFUNC( enum LSTAT,					HCD_Lock,					struct USBBase *usbbase, struct USB2_HCDNode *hn, STR file );
USB2_CREATEFUNC( void,							HCD_Unlock,					struct USBBase *usbbase, struct USB2_HCDNode *hn, STR file );
USB2_CREATEFUNC( enum VSTAT,					HCD_Valid,					struct USBBase *usbbase, struct USB2_HCDNode *hn, STR file );
USB2_CREATEFUNC( void,							HCD_Reply,					struct USBBase *usbbase, struct RealRequest *ioreq, STR file );

// -- Interface Node

USB2_CREATEFUNC( struct USB2_InterfaceNode *,	Interface_AllocNode,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih, STR file );
USB2_CREATEFUNC( enum FSTAT,					Interface_FreeNode,			struct USBBase *usbbase, struct USB2_InterfaceNode *in, STR file );
USB2_CREATEFUNC( enum LSTAT,					Interface_LockNode,			struct USBBase *usbbase, struct USB2_InterfaceNode *in, STR file );
USB2_CREATEFUNC( void,							Interface_UnlockNode,		struct USBBase *usbbase, struct USB2_InterfaceNode *in, STR file );
USB2_CREATEFUNC( enum VSTAT,					Interface_ValidNode,		struct USBBase *usbbase, struct USB2_InterfaceNode *in, STR file );

// -- Interface Header

USB2_CREATEFUNC( struct USB2_InterfaceHeader *,	Interface_AllocHeader,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig, U32 nr, STR file );
USB2_CREATEFUNC( enum FSTAT,					Interface_FreeHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih, STR file );
USB2_CREATEFUNC( enum LSTAT,					Interface_LockHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih, STR file );
USB2_CREATEFUNC( void,							Interface_UnlockHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih, STR file );
USB2_CREATEFUNC( enum VSTAT,					Interface_ValidHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih, STR file );

// -- Interface Group

USB2_CREATEFUNC( struct USB2_InterfaceGroup *,	Interface_AllocGroup,		struct USBBase *usbbase, struct USB2_ConfigNode *cn, STR file );
USB2_CREATEFUNC( enum FSTAT,					Interface_FreeGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig, STR file );
USB2_CREATEFUNC( enum LSTAT,					Interface_LockGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig, STR file );
USB2_CREATEFUNC( void,							Interface_UnlockGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig, STR file );
USB2_CREATEFUNC( enum VSTAT,					Interface_ValidGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig, STR file );

// -- IO

USB2_CREATEFUNC( S32,							IO_Do,						struct USBBase *usbbase, PTR ioreq, STR file );
USB2_CREATEFUNC( void,							IO_Stop,					struct USBBase *usbbase, PTR ioreq, STR file );
USB2_CREATEFUNC( void,							IO_Send,					struct USBBase *usbbase, PTR ioreq, STR file );
USB2_CREATEFUNC( void,							IO_Abort,					struct USBBase *usbbase, PTR ioreq, STR file );
USB2_CREATEFUNC( void,							IO_Restart,					struct USBBase *usbbase, PTR ioreq, STR file );

// -- IO Request

USB2_CREATEFUNC( struct RealRequest *,			IORequest_AllocList,		struct USBBase *usbbase, struct TagItem *taglist, STR file );
USB2_CREATEFUNC( enum FSTAT,					IORequest_Free,				struct USBBase *usbbase, struct RealRequest *ioreq, STR file );
USB2_CREATEFUNC( enum LSTAT,					IORequest_Lock,				struct USBBase *usbbase, struct RealRequest *ioreq, STR file );
USB2_CREATEFUNC( void,							IORequest_Unlock,			struct USBBase *usbbase, struct RealRequest *ioreq, STR file );
USB2_CREATEFUNC( enum VSTAT,					IORequest_Valid,			struct USBBase *usbbase, struct RealRequest *ioreq, STR file );
USB2_CREATEFUNC( void,							IORequest_Active_Add,		struct USBBase *usbbase, struct RealRequest *ioreq, STR file );
USB2_CREATEFUNC( void,							IORequest_Active_Sub,		struct USBBase *usbbase, struct RealRequest *ioreq, STR file );

// -- Memory

USB2_CREATEFUNC( PTR,							Memory_Alloc,				struct USBBase *usbbase, U32 memtype, S32 clear, STR file );
USB2_CREATEFUNC( void,							Memory_Free,				struct USBBase *usbbase, U32 memtype, PTR mem, U32 phyadr, STR file );

// -- Misc

USB2_CREATEFUNC( U32,							Misc_DoCommand,				struct USBBase *usbbase, struct CmdHeader *ch, STR file );

// -- MsgPort

USB2_CREATEFUNC( S32,							MsgPort_Init,				struct USBBase *usbbase, struct USB2_MsgPort *mp, STR file );
USB2_CREATEFUNC( void,							MsgPort_Free,				struct USBBase *usbbase, struct USB2_MsgPort *mp, STR file );
USB2_CREATEFUNC( PTR,							MsgPort_GetMsg,				struct USBBase *usbbase, struct USB2_MsgPort *mp, STR file );
USB2_CREATEFUNC( void,							MsgPort_PutMsg,				struct USBBase *usbbase, struct USB2_MsgPort *mp, PTR msg, STR file );
USB2_CREATEFUNC( void,							MsgPort_ReplyMsg,			struct USBBase *usbbase, PTR msg, STR file );
USB2_CREATEFUNC( void,							MsgPort_Reinit,				struct USBBase *usbbase, struct USB2_MsgPort *mp, STR file );

// -- Task

USB2_CREATEFUNC( struct USB2_TaskNode *,		Task_Alloc,					struct USBBase *usbbase, struct USB2_ASync *as, STR file );
USB2_CREATEFUNC( enum FSTAT,					Task_Free,					struct USBBase *usbbase, struct USB2_TaskNode *tn, STR file );
USB2_CREATEFUNC( enum LSTAT,					Task_Lock,					struct USBBase *usbbase, struct USB2_TaskNode *tn, STR file );
USB2_CREATEFUNC( void,							Task_Unlock,				struct USBBase *usbbase, struct USB2_TaskNode *tn, STR file );
USB2_CREATEFUNC( enum VSTAT,					Task_Valid,					struct USBBase *usbbase, struct USB2_TaskNode *tn, STR file );
USB2_CREATEFUNC( void,							Task_Stop_TN,				struct USBBase *usbbase, struct USB2_TaskNode *tn, STR file );
USB2_CREATEFUNC( void,							Task_Stop_DN,				struct USBBase *usbbase, struct USB2_DriverNode *dn, STR file );
USB2_CREATEFUNC( void,							Task_Stop_FN,				struct USBBase *usbbase, struct RealFunctionNode *fn, STR file );
USB2_CREATEFUNC( S32,							Task_AllocSignal,			struct USBBase *usbbase, struct USB2_Signal *sig, STR file );
USB2_CREATEFUNC( void,							Task_FreeSignal,			struct USBBase *usbbase, struct USB2_Signal *sig, STR file );
USB2_CREATEFUNC( void,							Task_ReallocSignal,			struct USBBase *usbbase, struct USB2_Signal *sig, STR file );

// -- Task Message

USB2_CREATEFUNC( struct USB2_TaskMsg *,			TaskMsg_Alloc,				struct USBBase *usbbase, STR file );
USB2_CREATEFUNC( enum FSTAT,					TaskMsg_Free,				struct USBBase *usbbase, struct USB2_TaskMsg *tm, STR file );
USB2_CREATEFUNC( enum LSTAT,					TaskMsg_Lock,				struct USBBase *usbbase, struct USB2_TaskMsg *tm, STR file );
USB2_CREATEFUNC( void,							TaskMsg_Unlock,				struct USBBase *usbbase, struct USB2_TaskMsg *tm, STR file );
USB2_CREATEFUNC( enum VSTAT,					TaskMsg_Valid,				struct USBBase *usbbase, struct USB2_TaskMsg *tm, STR file );

// -- Register

USB2_CREATEFUNC( struct RealRegister *,			Register_Alloc,				struct USBBase *usbbase, STR file );
USB2_CREATEFUNC( enum FSTAT,					Register_Free,				struct USBBase *usbbase, struct RealRegister *reg, STR file );
USB2_CREATEFUNC( enum LSTAT,					Register_Lock,				struct USBBase *usbbase, struct RealRegister *reg, STR file );
USB2_CREATEFUNC( void,							Register_Unlock,			struct USBBase *usbbase, struct RealRegister *reg, STR file );
USB2_CREATEFUNC( enum VSTAT,					Register_Valid,				struct USBBase *usbbase, struct RealRegister *reg, STR file );





#else






// -- Config

USB2_CREATEFUNC( struct USB2_ConfigNode *,		Config_Alloc,		struct USBBase *usbbase, struct RealFunctionNode *ftk );
USB2_CREATEFUNC( enum FSTAT,					Config_Free,		struct USBBase *usbbase, struct USB2_ConfigNode *cn );
USB2_CREATEFUNC( enum LSTAT,					Config_Lock,		struct USBBase *usbbase, struct USB2_ConfigNode *cn );
USB2_CREATEFUNC( void,							Config_Unlock,		struct USBBase *usbbase, struct USB2_ConfigNode *cn );
USB2_CREATEFUNC( enum VSTAT,					Config_Valid,		struct USBBase *usbbase, struct USB2_ConfigNode *cn );
USB2_CREATEFUNC( S32,							Config_Set,			struct USBBase *usbbase, struct RealFunctionNode *fkt, struct RealRequest *IOReq, U32 nr );

// -- Driver

USB2_CREATEFUNC( struct USB2_DriverNode *,		Driver_Alloc,		struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_ASync *as );
USB2_CREATEFUNC( enum FSTAT,					Driver_Free,		struct USBBase *usbbase, struct USB2_DriverNode *dn );
USB2_CREATEFUNC( enum LSTAT,					Driver_Lock,		struct USBBase *usbbase, struct USB2_DriverNode *dn );
USB2_CREATEFUNC( void,							Driver_Unlock,		struct USBBase *usbbase, struct USB2_DriverNode *dn );
USB2_CREATEFUNC( enum VSTAT,					Driver_Valid,		struct USBBase *usbbase, struct USB2_DriverNode *dn );

// -- EndPoint

USB2_CREATEFUNC( struct USB2_EndPointNode *,	EndPoint_Alloc,		struct USBBase *usbbase, struct RealFunctionNode *fn, struct USB2_InterfaceNode *in, struct USB2_EndPoint_Desc *epdsc );
USB2_CREATEFUNC( enum FSTAT,					EndPoint_Free,		struct USBBase *usbbase, struct USB2_EndPointNode *ep );
USB2_CREATEFUNC( enum LSTAT,					EndPoint_Lock,		struct USBBase *usbbase, struct USB2_EndPointNode *ep );
USB2_CREATEFUNC( void,							EndPoint_Unlock,	struct USBBase *usbbase, struct USB2_EndPointNode *ep );
USB2_CREATEFUNC( enum VSTAT,					EndPoint_Valid,		struct USBBase *usbbase, struct USB2_EndPointNode *ep );

// -- EndPoint Resource

USB2_CREATEFUNC( struct RealEndPointResource *,	EndPointRes_Alloc,			struct USBBase *usbbase );
USB2_CREATEFUNC( enum FSTAT,					EndPointRes_Free,			struct USBBase *usbbase, struct RealEndPointResource *epr );
USB2_CREATEFUNC( enum LSTAT,					EndPointRes_Lock,			struct USBBase *usbbase, struct RealEndPointResource *epr );
USB2_CREATEFUNC( void,							EndPointRes_Unlock,			struct USBBase *usbbase, struct RealEndPointResource *epr );
USB2_CREATEFUNC( enum VSTAT,					EndPointRes_Valid,			struct USBBase *usbbase, struct RealEndPointResource *epr );
USB2_CREATEFUNC( struct USB2_EPResource * VARARGS68K, EndPointRes_ObtainTags, struct USBBase *usbbase, struct RealRegister *reg, ... );
USB2_CREATEFUNC( struct USB2_EPResource *,		EndPointRes_ObtainList,		struct USBBase *usbbase, struct RealRegister *reg, struct TagItem *taglist );
USB2_CREATEFUNC( enum FSTAT,					EndPointRes_Release,		struct USBBase *usbbase, struct USB2_EPResource *epr );
USB2_CREATEFUNC( U32,							EndPointRes_Destall,		struct USBBase *usbbase, struct USB2_EPResource *epr );

// -- FDriver 

USB2_CREATEFUNC( struct USB2_FktDriverNode *,	FDriver_Alloc,				struct USBBase *usbbase );
USB2_CREATEFUNC( enum FSTAT,					FDriver_Free,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn );
USB2_CREATEFUNC( enum LSTAT,					FDriver_Lock,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn );
USB2_CREATEFUNC( void,							FDriver_Unlock,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn );
USB2_CREATEFUNC( enum VSTAT,					FDriver_Valid,				struct USBBase *usbbase, struct USB2_FktDriverNode *fdn );

// -- Function 

USB2_CREATEFUNC( struct RealFunctionNode *,		Function_Alloc,				struct USBBase *usbbase, struct USB2_HCDNode *hn, struct USB2_ASync *as, U32 Speed, U32 Tier );
USB2_CREATEFUNC( enum FSTAT,					Function_Free,				struct USBBase *usbbase, struct RealFunctionNode *fn );
USB2_CREATEFUNC( enum LSTAT,					Function_Lock,				struct USBBase *usbbase, struct RealFunctionNode *fn );
USB2_CREATEFUNC( void,							Function_Unlock,			struct USBBase *usbbase, struct RealFunctionNode *fn );
USB2_CREATEFUNC( enum VSTAT,					Function_Valid,				struct USBBase *usbbase, struct RealFunctionNode *fn );

// -- HCD

USB2_CREATEFUNC( struct USB2_HCDNode *,			HCD_Alloc,					struct USBBase *usbbase, U32 hcdtype );
USB2_CREATEFUNC( enum FSTAT,					HCD_Free,					struct USBBase *usbbase, struct USB2_HCDNode *hn );
USB2_CREATEFUNC( enum LSTAT,					HCD_Lock,					struct USBBase *usbbase, struct USB2_HCDNode *hn );
USB2_CREATEFUNC( void,							HCD_Unlock,					struct USBBase *usbbase, struct USB2_HCDNode *hn );
USB2_CREATEFUNC( enum VSTAT,					HCD_Valid,					struct USBBase *usbbase, struct USB2_HCDNode *hn );
USB2_CREATEFUNC( void,							HCD_Reply,					struct USBBase *usbbase, struct RealRequest *ioreq );

// -- Interface Node

USB2_CREATEFUNC( struct USB2_InterfaceNode *,	Interface_AllocNode,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih );
USB2_CREATEFUNC( enum FSTAT,					Interface_FreeNode,			struct USBBase *usbbase, struct USB2_InterfaceNode *in );
USB2_CREATEFUNC( enum LSTAT,					Interface_LockNode,			struct USBBase *usbbase, struct USB2_InterfaceNode *in );
USB2_CREATEFUNC( void,							Interface_UnlockNode,		struct USBBase *usbbase, struct USB2_InterfaceNode *in );
USB2_CREATEFUNC( enum VSTAT,					Interface_ValidNode,		struct USBBase *usbbase, struct USB2_InterfaceNode *in );

// -- Interface Header

USB2_CREATEFUNC( struct USB2_InterfaceHeader *,	Interface_AllocHeader,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig, U32 nr );
USB2_CREATEFUNC( enum FSTAT,					Interface_FreeHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih );
USB2_CREATEFUNC( enum LSTAT,					Interface_LockHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih );
USB2_CREATEFUNC( void,							Interface_UnlockHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih );
USB2_CREATEFUNC( enum VSTAT,					Interface_ValidHeader,		struct USBBase *usbbase, struct USB2_InterfaceHeader *ih );

// -- Interface Group

USB2_CREATEFUNC( struct USB2_InterfaceGroup *,	Interface_AllocGroup,		struct USBBase *usbbase, struct USB2_ConfigNode *cn );
USB2_CREATEFUNC( enum FSTAT,					Interface_FreeGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig );
USB2_CREATEFUNC( enum LSTAT,					Interface_LockGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig );
USB2_CREATEFUNC( void,							Interface_UnlockGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig );
USB2_CREATEFUNC( enum VSTAT,					Interface_ValidGroup,		struct USBBase *usbbase, struct USB2_InterfaceGroup *ig );

// -- IO

USB2_CREATEFUNC( S32,							IO_Do,						struct USBBase *usbbase, PTR ioreq );
USB2_CREATEFUNC( void,							IO_Stop,					struct USBBase *usbbase, PTR ioreq );
USB2_CREATEFUNC( void,							IO_Send,					struct USBBase *usbbase, PTR ioreq );
USB2_CREATEFUNC( void,							IO_Abort,					struct USBBase *usbbase, PTR ioreq );
USB2_CREATEFUNC( void,							IO_Restart,					struct USBBase *usbbase, PTR ioreq );

// -- IO Request

USB2_CREATEFUNC( struct RealRequest *,			IORequest_AllocList,		struct USBBase *usbbase, struct TagItem *taglist );
USB2_CREATEFUNC( enum FSTAT,					IORequest_Free,				struct USBBase *usbbase, struct RealRequest *ioreq );
USB2_CREATEFUNC( enum LSTAT,					IORequest_Lock,				struct USBBase *usbbase, struct RealRequest *ioreq );
USB2_CREATEFUNC( void,							IORequest_Unlock,			struct USBBase *usbbase, struct RealRequest *ioreq );
USB2_CREATEFUNC( enum VSTAT,					IORequest_Valid,			struct USBBase *usbbase, struct RealRequest *ioreq );
USB2_CREATEFUNC( void,							IORequest_Active_Add,		struct USBBase *usbbase, struct RealRequest *ioreq );
USB2_CREATEFUNC( void,							IORequest_Active_Sub,		struct USBBase *usbbase, struct RealRequest *ioreq );

// -- Memory

USB2_CREATEFUNC( PTR,							Memory_Alloc,				struct USBBase *usbbase, U32 memtype, S32 clear );
USB2_CREATEFUNC( void,							Memory_Free,				struct USBBase *usbbase, U32 memtype, PTR mem, U32 phyadr );

// -- Misc

USB2_CREATEFUNC( U32,							Misc_DoCommand,				struct USBBase *usbbase, struct CmdHeader *ch );

// -- MsgPort

USB2_CREATEFUNC( S32,							MsgPort_Init,				struct USBBase *usbbase, struct USB2_MsgPort *mp );
USB2_CREATEFUNC( void,							MsgPort_Free,				struct USBBase *usbbase, struct USB2_MsgPort *mp );
USB2_CREATEFUNC( PTR,							MsgPort_GetMsg,				struct USBBase *usbbase, struct USB2_MsgPort *mp );
USB2_CREATEFUNC( void,							MsgPort_PutMsg,				struct USBBase *usbbase, struct USB2_MsgPort *mp, PTR msg );
USB2_CREATEFUNC( void,							MsgPort_ReplyMsg,			struct USBBase *usbbase, PTR msg );
USB2_CREATEFUNC( void,							MsgPort_Reinit,				struct USBBase *usbbase, struct USB2_MsgPort *mp );

// -- Task

USB2_CREATEFUNC( struct USB2_TaskNode *,		Task_Alloc,					struct USBBase *usbbase, struct USB2_ASync *as );
USB2_CREATEFUNC( enum FSTAT,					Task_Free,					struct USBBase *usbbase, struct USB2_TaskNode *tn );
USB2_CREATEFUNC( enum LSTAT,					Task_Lock,					struct USBBase *usbbase, struct USB2_TaskNode *tn );
USB2_CREATEFUNC( void,							Task_Unlock,				struct USBBase *usbbase, struct USB2_TaskNode *tn );
USB2_CREATEFUNC( enum VSTAT,					Task_Valid,					struct USBBase *usbbase, struct USB2_TaskNode *tn );
USB2_CREATEFUNC( void,							Task_Stop_TN,				struct USBBase *usbbase, struct USB2_TaskNode *tn );
USB2_CREATEFUNC( void,							Task_Stop_DN,				struct USBBase *usbbase, struct USB2_DriverNode *dn );
USB2_CREATEFUNC( void,							Task_Stop_FN,				struct USBBase *usbbase, struct RealFunctionNode *fn );
USB2_CREATEFUNC( S32,							Task_AllocSignal,			struct USBBase *usbbase, struct USB2_Signal *sig );
USB2_CREATEFUNC( void,							Task_FreeSignal,			struct USBBase *usbbase, struct USB2_Signal *sig );
USB2_CREATEFUNC( void,							Task_ReallocSignal,			struct USBBase *usbbase, struct USB2_Signal *sig );

// -- Task Message

USB2_CREATEFUNC( struct USB2_TaskMsg *,			TaskMsg_Alloc,				struct USBBase *usbbase );
USB2_CREATEFUNC( enum FSTAT,					TaskMsg_Free,				struct USBBase *usbbase, struct USB2_TaskMsg *tm );
USB2_CREATEFUNC( enum LSTAT,					TaskMsg_Lock,				struct USBBase *usbbase, struct USB2_TaskMsg *tm );
USB2_CREATEFUNC( void,							TaskMsg_Unlock,				struct USBBase *usbbase, struct USB2_TaskMsg *tm );
USB2_CREATEFUNC( enum VSTAT,					TaskMsg_Valid,				struct USBBase *usbbase, struct USB2_TaskMsg *tm );

// -- Register

USB2_CREATEFUNC( struct RealRegister *,			Register_Alloc,				struct USBBase *usbbase );
USB2_CREATEFUNC( enum FSTAT,					Register_Free,				struct USBBase *usbbase, struct RealRegister *reg );
USB2_CREATEFUNC( enum LSTAT,					Register_Lock,				struct USBBase *usbbase, struct RealRegister *reg );
USB2_CREATEFUNC( void,							Register_Unlock,			struct USBBase *usbbase, struct RealRegister *reg );
USB2_CREATEFUNC( enum VSTAT,					Register_Valid,				struct USBBase *usbbase, struct RealRegister *reg );

#endif
