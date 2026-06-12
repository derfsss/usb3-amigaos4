
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_TASK_State_H
#define INC_PRIVATE_TASK_State_H

/***************************************************************************/

// --
// Task Tags, only used internaly

// Note: its not for a real taglist.. as some tag my be on
// stack and other in regs.. so only Read 4+4 bytes (ID:Data)
// via stdarg.h

enum TaskTags
{
	TASK_Tag_Dummy				= 0xac000000,
	TASK_Tag_Get_TN,			// Get Pointer to put tn (return)
	TASK_Tag_UserData,
	TASK_Tag_Prioity,
	TASK_Tag_Func_Init,
	TASK_Tag_Func_Main,
	TASK_Tag_Func_Free,
	TASK_Tag_HCDNode,
	TASK_Tag_ASync,
	TASK_Tag_Name,
	TASK_Tag_Type,				// enum TaskType
	TASK_Tag_InternSize,		// size of intern data/struct size
	USB3_WaitForTermination,	// 
};

// --

enum TaskState
{
	TASK_State_Unset = 0,
	TASK_State_Starting,
	TASK_State_Running,
	TASK_State_Stopping,
	TASK_State_Stopped,
	TASK_State_Last,
};

// --

enum TaskType
{
	TASK_Type_Unset,		// hmm error I guess
	TASK_Type_HCD,			// HCD
//	TASK_Type_HUB,			// Root HUB
//	TASK_Type_DEV,			// Bind Device
	TASK_Type_FKT,			// Bind Function
//	TASK_Type_IFC,			// Bind Interface
	TASK_Type_Last
};

// --
// Return codes for TASK_START()

#define TASKRETURN_MASK			0xffff0000UL	// ID Mask
#define TASKRETURN_CODE			0x0000ffffUL	// Return Mask
#define TASKRETURN_ID			0xacdc0000UL	// Upper ID
#define TASKRETURN_FLAG			0x00008000UL	// Error Flag .. negativ for 16bit

#define TASKRETURN_OK(x)		(( TASKRETURN_ID ) | ( x & TASKRETURN_CODE ))
#define TASKRETURN_ERR(x)		(( TASKRETURN_ID ) | ( x & TASKRETURN_CODE ) | ( TASKRETURN_FLAG ))

// Verify its a valid ReturnCode
#define IS_TASKRETURN(x)		(( x & TASKRETURN_MASK ) == TASKRETURN_ID )
// Get Return Code
#define GET_TASKRETURN(x)		( x & TASKRETURN_CODE )

S32 myIS_TASKRETURN_ERR( U32 value );
U32 myGET_TASKRETURN_CODE( U32 value );

enum TaskReturn
{
	// Task failed to start
	// most likely an stack error
	TASK_Return_Stack_Error			= TASKRETURN_ERR( 10998 ),

	// Task started okay
	TASK_Return_Stack_Okay			= TASKRETURN_OK( 10999 ),

	// Driver failed to start
	TASK_Return_Error				= TASKRETURN_ERR( 0 ),

	// Driver started
	TASK_Return_Okay				= TASKRETURN_OK( 1 ),

	// Driver started, but unsupported
	TASK_Return_Unsupported			= TASKRETURN_OK( 2 ),
};

// --

struct USB3_TaskNode
{
	struct USB3_Node				tn_Node;
	// -- 
	U32								tn_StructID;
	S32								tn_Locks;
	U16								tn_Detach;
	U16								tn_FreeMe;
	//	struct USB3_TaskNode *			tn_TaskAdr; 
	struct USB3_ASync *				tn_ASync;
	// --
	struct Task *					tn_TaskAdr;
	struct Task *					tn_Parent;
	U32								tn_Mask;
	enum TaskState					tn_State;
	enum TaskType					tn_Type;
	PTR								tn_Owner;			// Points to a struct like 'struct USB3_HCDNode' see Type
};

// --

#if 0

struct CmdHeader
{
	struct Message		cm_Message;
	enum USB3_Cmd		cm_Command;
	struct USBBase *	cm_usbbase;
	U32					cm_Result;
	struct Task *		cm_Task;
	U32					cm_Mask;
};

#endif

struct USB3_DriverNode;

struct USB3_TaskMsg
{
	struct CmdHeader				tm_Cmd;				// 
	// -- 
	U32								tm_StructID;
	S32								tm_Locks;
	U16								tm_Detach;
	U16								tm_FreeMe;
	struct USB3_TaskNode *			tm_Task; 
	struct USB3_ASync *				tm_ASync;
	// --


	U32								tm_InternSize;
	S32							( * tm_Func_Init )	( struct USBBase *usbbase, struct USB3_DriverNode *dn, PTR in );
	void						( * tm_Func_Entry )	( struct USBBase *usbbase, struct USB3_DriverNode *dn, PTR in );
	void						( * tm_Func_Free )	( struct USBBase *usbbase, struct USB3_DriverNode *dn, PTR in );
	S32								tm_Prioity;				// 
	S32								tm_StackSize;
	PTR								tm_UserData;			// 
	enum TaskType					tm_Type;				// enum TaskType
	struct USB3_HCDNode *			tm_HCD;					// hn
	struct USB3_DriverNode *		tm_Driver;				// dn
	struct RealFunctionNode *		tm_Function;			// fn
	struct USB3_TaskNode **			tm_Get_TaskNode;		// User ptr where to store TN
	// --



	STR								tm_Name;
	PTR								tm_Data;
	struct TimeRequest				tm_TimeRequest;
	// --
	S8								tm_TimeReq_Active;
//	S8								tm_Wait;			// TASK_STOP_TN() wait for Shutdown?
	S8								tm_Keep;			// TASK_STOP_TN() no free, its active
	S8								tm_Added;			// TASK_STOP_TN() Added
	S8								tm_Wait_For_Term;	// TASK_START() wait untell finish
};

// --
/***************************************************************************/

#endif // INC_PRIVATE_TASK_State_H
