
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef DRIVERS_msddisk_msddisk_H
#define DRIVERS_msddisk_msddisk_H

// --

#define MAX_MSD_UNITS			32

#define MSD_Device_Name			"usb3disk.device"
#define MSD_Device_Version		1
#define MSD_Device_Revision		0

#define MSD_SubClass_QIC		0x03	// Obsolete QIC-157
#define MSD_SubClass_UFI		0x04	// Floppy Disk
#define MSD_SubClass_SFF		0x05	// Obsolete SFF-8070i
#define MSD_SubClass_SCSI		0x06	//

#define MSD_Protocol_CBINT		0x00	// With Cmd Completion Interrupt
#define MSD_Protocol_CB			0x01	// Without Cmd Completion Interrupt
#define MSD_Protocol_Bulk		0x50

#define REQCODE_MassStorage_GetMaxLUN	254
#define REQCODE_MassStorage_Reset		255

#define SCSI_DA_READ_16			0x88
#define SCSI_DA_WRITE_16		0x8a

// --

/*
** SCSI Commands
**
** 0x00 : Test Unit Ready
** 0x03 : Request Sense
** 0x04 : Format Unit
** 0x12 : Inquiry
** 0x1B : Start Stop Unit (Eject Device)
** 0x1E : Prevent Allow Medium Removal
** 0x23 : Read Format Capacities
** 0x25 : Read Capacity
** 0x28 : Read (10)
** 0x2A : Write (10)
** 0x2B : Seek (10)
** 0x2E : Write and Verify (10)
** 0x2F : Verify (10)
** 0x35 : Synchronize Cache
** 0x3B : Write Buffer
** 0x3C : Read Buffer
** 0x43 : Read TOC/PMA/ATIP
** 0x46 : Get Configuration
** 0x4A : Get Event Status Notification
** 0x51 : Read Disc Information
** 0x52 : Read Track Information
** 0x53 : Reserve Track
** 0x54 : Send OPC Information
** 0x55 : Mode Select (10)
** 0x58 : Repair Track
** 0x5A : Mode Sense (10)
** 0x5B : Close Track Session
** 0x5C : Read Buffer Caoacity
** 0x5D : Send CUE Sheet
** 0xA0 : Report LUNs
** 0xA1 : Blank
** 0xA2 : Security Protocol In
** 0xA3 : Send Key
** 0xA4 : Report Key
** 0xA6 : Load/Unlock Medium
** 0xA7 : Set Read AHead
** 0xA8 : Read (12)
** 0xAA : Write (12)
** 0xAB / 0x01 : Read Media Serial Number / Service Action in (12)
** 0xAC : Get Performance
** 0xAD : Read Disk Structure
** 0xB5 : Security Protocol Out
** 0xB6 : Set Streaming
** 0xB9 : Read CD MSF
** 0xBB : Set CD Speed
** 0xBD : Mechanism Status
** 0xBE : Read CD
** 0xBF : Send Disc Structure
**
*/

// --

#pragma pack(1)

// MSD Command Block Wrapper
struct MSDCBW
{
	U32		cbw_Signature;
	U32		cbw_Tag;
	U32		cbw_DataLength;
	U8		cbw_Flags;
	U8		cbw_LUN;
	U8		cbw_CBLength;
	U8		cbw_CB[16];
};

#define MSDCBW_Signature			0x43425355		// LE "CBSU" -> BE "USBC"
#define MSDCBW_Flags_DataIn			0x80
#define MSDCBW_Flags_DataOut		0x00

// MSD Command Status Wrapper

struct MSDCSW
{
	U32		csw_Signature;
	U32		csw_Tag;
	U32		csw_DataResidue;
	U8		csw_Status;
};

#define MSDCSW_Signature			0x53425355		// LE "SBSU" -> BE "USBS"
#define MSDCSW_Status_CmdPassed		0
#define MSDCSW_Status_CmdFailed		1
#define MSDCSW_Status_PhaseError	2

#pragma pack(0)

// --

struct DoBulkResult
{
	U8		csw_Valid;				// TRUE if a CSW was read & parsed
	U8		csw_Status;				// MSDCSW_Status_*
	U8		csw_SigOK;				// optional
	U8		csw_TagOK;				// optional
	U32		csw_DataResidue;		// host order, bytes not transferred
	U32		csw_DataActual;			// host order, bytes transferred ( requested - residue )
};

struct DoBulkStruct
{
	struct DoBulkResult *dbs_Result;
	U32		dbs_Flags;
	// -- Send Command
	U32		dbs_Command_Length;
	// -- Optional : Data buffer for Read/Write
	PTR		dbs_Data;
	U32		dbs_Data_Length;
	// -- On Error we can Fill Sense Buffer
	PTR		dbs_Sense;
	U32		dbs_Sense_Length;
	// -- 
	U8		cmd[16];
};

#define DOBULKF_DIR_IN		0x00	// device -> host
#define DOBULKF_DIR_OUT		0x01	// host -> device
#define DOBULKF_DIR_MASK	0x01

#define DOBULKF_AUTOSENSE	0x02	// perform REQUEST SENSE on CSW=CmdFailed
#define DOBULKF_SHORT_OK	0x04	// tolerate short data (residue) without error

// --

struct MSDBase
{
	struct Library					msdbase_Library;
	U16								msdbase_Pad;
	PTR								msdbase_SegList;
	struct USBBase *				msdbase_usbbase;				// USB stack
	struct USB2_Semaphore 			msdbase_MSDDisk_Semaphore;
	PTR								msdbase_MSDDisk_Units[MAX_MSD_UNITS];	// struct MSDDisk's
};

// --

struct MSDDevice
{
	struct USB2_Node				msddev_Node;
	struct USB2_DriverMessage *		msddev_StartMsg;
	struct MSDBase *				msddev_MSDBase;
	struct USB2_Header				msddev_MSDDisk_Header;			// struct MSDDisk's
	struct USB2_MsgPort				msddev_MsgPort_Begin;
	struct USB2_MsgPort				msddev_MsgPort_Abort;
	struct RealRegister *			msddev_Register;
	struct USB2_EPResource *		msddev_Res_Bulk_Out;
	struct USB2_EPResource *		msddev_Res_Bulk_In;
	struct USB2_EPResource *		msddev_Res_Control;
	U32								msddev_Bulk_TagID;
	U32								msddev_InterfaceNr;

	// -- 
	U8								Running;
	U8								msddev_Detached;
};

// --
// one pr. lun

struct MSDDisk
{
	struct USB2_Node				msddisk_Node;
	struct MSDDevice *				msddisk_MSDDev;
	struct MSDBase *				msddisk_MSDBase;
	struct DriveGeometry			msddisk_Geometry;				// Drive Geometry
	U32								msddisk_DiskChangeCount;		// Number of disk change
	U32								msddisk_GeoChangeCount;
	U32								msddisk_Block_Size;
	U32								msddisk_Block_Mask;
	U32								msddisk_Block_Shift;
	U64								msddisk_TotalBlocks;
	struct MSDCSW					msddisk_CSW;
	struct MSDCBW					msddisk_CBW;
	U32								msddisk_Lun;

	S32								msddisk_OpenCnt;
	struct USB2_MsgPort *			msddisk_MsgPort_Begin;
	struct USB2_MsgPort *			msddisk_MsgPort_Abort;
	U32								msddisk_UnitNr;

	struct USB2_Semaphore 			msddisk_MSDUnit_Semaphore;
	struct USB2_Header				msddisk_MSDUnit_ChangeList;
	struct USB2_Header				msddisk_MSDUnit_StatList;

	// -- Config
	U8								msddisk_DeviceType;				// Disk or CDRom
	U8								msddisk_Removable;
	U8								msddisk_InRecovery;
	U8								msddisk_Announced;
};

// --

enum PhaseStat
{
	PS_Unset,
	PS_Okay,		// No Error
	PS_Stall,		// Data Stage error
	PS_Error,		// Transport error/timout/etc
};

#ifndef MSDCBW_LEN
#define MSDCBW_LEN 31
#endif

#ifndef MSDCSW_LEN
#define MSDCSW_LEN 13
#endif

_Static_assert(sizeof(struct MSDCBW) == MSDCBW_LEN, "MSDCBW must be 31 bytes (packed)");
_Static_assert(sizeof(struct MSDCSW) == MSDCSW_LEN, "MSDCSW must be 13 bytes (packed)");

// --

/*[ Protos ]****************************************************************/

S32 MSD_Cmd_000E_TD_ChangeState( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_000F_TD_ProtStatus( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_001C_HD_SCSICmd( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_0002_CMD_Read( struct USBBase *usbbase, struct MSDDevice *msddev, struct IOStdReq *ioreq );
S32 MSD_Cmd_0003_CMD_Write( struct USBBase *usbbase, struct MSDDevice *msddev, struct IOStdReq *ioreq );
S32 MSD_Cmd_0009_TD_Motor( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_0012_TD_GetDriveType( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_0014_TD_AddChangeInt( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_0015_TD_RemChangeInt( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_0016_TD_GetGeometry( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_0018_TD_Read64( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_4000_NSCmd_DeviceQuery( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_A004_NSCmd_GetGeometry64( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_A005_NSCmd_ChangeUnit( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_A006_NSCmd_AddStatCallBack( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_A007_NSCmd_RemStatCallBack( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );
S32 MSD_Cmd_C000_NSCmd_Read64( struct USBBase *usbbase, struct MSDDevice *msddev UNUSED, struct IOStdReq *ioreq );

enum PhaseStat MSD_Bulk_Cmd_Phase( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkStruct *dbs );
enum PhaseStat MSD_Bulk_Cmd_Data( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkStruct *dbs );
enum PhaseStat MSD_Bulk_Cmd_Status( struct USBBase *usbbase, struct MSDDisk *msddisk );
void MSD_DoBulk_Recovery( struct USBBase *usbbase, struct MSDDisk *msddisk );
S32 MSD_Disk_DoBulk( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkStruct *dbs );

void MSD_Manager_AbortIO( struct DeviceManagerInterface *Self UNUSED, struct IOStdReq *ioreq UNUSED );
void MSD_Manager_BeginIO( struct DeviceManagerInterface *Self, struct IOStdReq *ioreq );
PTR MSD_Manager_Close( struct DeviceManagerInterface *Self, struct IORequest *ioreq );
PTR MSD_Manager_Expunge( struct DeviceManagerInterface *Self UNUSED );
struct MSDBase *MSD_Manager_Init( struct MSDBase *MSDBase, PTR seglist, struct ExecIFace *IExec UNUSED );
U32 MSD_Manager_Obtain( struct DeviceManagerInterface *Self );
S32 MSD_Manager_Open( struct DeviceManagerInterface *Self, struct IORequest *ioreq, U32 unit_number, U32 flags UNUSED );
U32 MSD_Manager_Release( struct DeviceManagerInterface *Self );

S32 MSD_SCSI_Inquiry(  struct USBBase *usbbase, struct MSDDisk *msddisk, PTR mem, U32 len );
S32 MSD_SCSI_Read10( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkResult *dbr, U32 blk_start, U32 blk_blocks, PTR mem, U32 *actual_ptr );
S32 MSD_SCSI_Read16( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkResult *dbr, U64 blk_start, U32 blk_blocks, PTR mem, U32 *actual_ptr );
S32 MSD_SCSI_ReadCapacity10( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkResult *dbr, PTR mem, U32 len );
S32 MSD_SCSI_RequestSense( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkResult *dbr, PTR mem, U32 reqlen );
S32 MSD_SCSI_TestUnitReady( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkResult *dbr );
S32 MSD_SCSI_Write10( struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkResult *dbr, U32 blk_start, U32 blk_blocks, PTR mem, U32 *actual_ptr );
S32 MSD_Disk_TestUnitReady( struct USBBase *usbbase, struct MSDDisk *msddisk );

void MSD_Main( struct USBBase *usbbase, struct MSDDevice *msddev );
void MSD_Main_1_Normal( struct USBBase *usbbase, struct MSDDevice *msddev );
void MSD_Main_2_Stopping( struct USBBase *usbbase, struct MSDDevice *msddev );
void MSD_Main__myHandle_Abort( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED );
void MSD_Main__myHandle_Begin( struct USBBase *usbbase, struct MSDDevice *msddev );
void MSD_Main__myHandle_Stack( struct USBBase *usbbase UNUSED, struct MSDDevice *msddev UNUSED );

S32						MSD_CreateDevice(		struct USBBase *usbbase );

struct MSDDevice *		MSD_Device_Alloc(		struct USBBase *usbbase, struct USB2_DriverMessage *msg );
void					MSD_Device_Free(		struct USBBase *usbbase, struct MSDDevice *msddev );
U32						MSD_Device_GetMaxLun(	struct USBBase *usbbase, struct MSDDevice *msddev );

struct MSDDisk *		MSD_Disk_Alloc(			struct USBBase *usbbase, struct MSDDevice *msddev, U32 lun );
void					MSD_Disk_Free(			struct USBBase *usbbase, struct MSDDisk *msddisk );
S32						MSD_Disk_DoBulk(		struct USBBase *usbbase, struct MSDDisk *msddisk, struct DoBulkStruct *dbs );
S32						MSD_Disk_TestUnitReady(	struct USBBase *usbbase, struct MSDDisk *msddisk );
void					MSD_Disk_Detach(		struct USBBase *usbbase, struct MSDDisk *msddisk );

// --

#endif // DRIVERS_msddisk_msddisk_H
