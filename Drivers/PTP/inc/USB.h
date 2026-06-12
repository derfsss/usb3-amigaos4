
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_ALL_H
#error Include "inc/All.h" first
#endif

#ifndef INC_USB_H
#define INC_USB_H

// --

enum CmdType
{
	CMDTYPE_Cmd_None_Stat,			// Header Status

	CMDTYPE_Cmd_ReadMem_Stat,		// Header Memory Status
	CMDTYPE_Cmd_ReadFile_Stat,		// Header File Status

	CMDTYPE_Cmd_SendMem_Stat,		// Header Memory Status
	CMDTYPE_Cmd_SendFile_Stat,		// Header File Status
};

enum CmdStage
{
	// Send cmd
	CMDSTAGE_Send_Header1,		// working
	CMDSTAGE_Send_Header2,

	// Read into a memory buffer
	CMDSTAGE_Read_Memory1,		// working, but need more work
	CMDSTAGE_Read_Memory2,
	CMDSTAGE_Read_Memory3,

	// Read into a disk file
	CMDSTAGE_Read_File1,		// not implemented yet
	CMDSTAGE_Read_File2,
	CMDSTAGE_Read_File3,

	// Write from a memory buffer
	CMDSTAGE_Send_Memory1,		// not implemented yet
	CMDSTAGE_Send_Memory2,

	// Write into a disk file
	CMDSTAGE_Send_File1,		// not implemented yet
	CMDSTAGE_Send_File2,

	// Read cmd Status
	CMDSTAGE_Read_Responce1,	// working
	CMDSTAGE_Read_Responce2,
};

struct USB_Struct;

struct USB_Command
{
	struct Node						ucmd_Node;
	enum CmdType					ucmd_Type;
	enum CmdStage					ucmd_State;
	U32								ucmd_Command;
	struct PTP_Container			ucmd_Container;
	U32								ucmd_Data_Length;
	U8 *							ucmd_Data_Buffer;
	U32								ucmd_Data_Current;
	void							( *ucmd_Next )( struct USB_Struct *, struct USB_Command * );
	void							( *ucmd_Function )( struct USB_Struct *, struct USB_Command * );
};

// --

struct USB_Struct
{
	S32								us_Detached;
	S32								us_Running;
	U32								us_TransactionID;
	U32								us_SessionID;
	struct USB3_Register *			us_Register;
	struct USB3_DriverMessage *		us_StartMsg;
	struct USB3_EPResource *		us_Res_BulkIn;
	struct USB3_EPResource *		us_Res_BulkOut;
	struct USB3_EPResource *		us_Res_Control;
	struct USB3_EPResource *		us_Res_Interrupt;
	struct List						us_CmdList;
	S32								us_CmdListCnt;
	struct MsgPort *				us_CmdMsgPort;
	struct CameraInfoNode			us_CameraNode;
	struct List						us_USBList;
	struct USB_Command *			us_USBActive;
	U32								us_Errors_Int;
	U32								us_Errors_Bulk;
};

// --

#endif
