
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef DEVICES_USB3_H
#define DEVICES_USB3_H

/***************************************************************************/

#include "proto/exec.h"

#include <stdint.h>

/***************************************************************************/

#pragma pack(1)

/***************************************************************************/

#ifndef LE_SWAP16
#define LE_SWAP16(x)	( (( (x) & 0x00ffU ) << 8 ) \
						| (( (x) & 0xff00U ) >> 8 ) )
#endif

#ifndef LE_SWAP32
#define LE_SWAP32(x)	( (( (x) & 0x000000ffUL ) << 24 ) \
						| (( (x) & 0x0000ff00UL ) << 8  ) \
						| (( (x) & 0x00ff0000UL ) >> 8  ) \
						| (( (x) & 0xff000000UL ) >> 24 ) )
#endif

#ifndef LE_SWAP64
#define LE_SWAP64(x)	( (( (x) & 0x00000000000000ffULL ) << 56 ) \
						| (( (x) & 0x000000000000ff00ULL ) << 40 ) \
						| (( (x) & 0x0000000000ff0000ULL ) << 24 ) \
						| (( (x) & 0x00000000ff000000ULL ) <<  8 ) \
						| (( (x) & 0x000000ff00000000ULL ) >>  8 ) \
						| (( (x) & 0x0000ff0000000000ULL ) >> 24 ) \
						| (( (x) & 0x00ff000000000000ULL ) >> 40 ) \
						| (( (x) & 0xff00000000000000ULL ) >> 56 ) )
#endif

#ifndef BE_SWAP16
#define BE_SWAP16(x)	(x)
#endif

#ifndef BE_SWAP32
#define BE_SWAP32(x)	(x)
#endif

#ifndef BE_SWAP64
#define BE_SWAP64(x)	(x)
#endif

#ifndef USB2_ID
typedef uint64_t		USB2_ID;
#endif

#ifndef MIN
#define MIN(x,y)		((( x ) < ( y )) ? ( x ) : ( y ))
#endif

#ifndef MAX
#define MAX(x,y)		((( x ) > ( y )) ? ( x ) : ( y ))
#endif

#ifndef U8
typedef uint8_t			U8;
#endif

#ifndef U16
typedef uint16_t		U16;
#endif

#ifndef U32
typedef uint32_t		U32;
#endif

#ifndef U64
typedef uint64_t		U64;
#endif

#ifndef S8
typedef int8_t			S8;
#endif

#ifndef S16
typedef int16_t			S16;
#endif

#ifndef S32
typedef int32_t			S32;
#endif

#ifndef S64
typedef int64_t			S64;
#endif

#ifndef PTR
typedef void *			PTR;
#endif

#ifndef STR
typedef char *			STR;
#endif

/***************************************************************************/

enum USBCLASS
{
	USBCLASS_None = 0x00,
	USBCLASS_Audio,
	USBCLASS_CDC_Control,
	USBCLASS_HID,
	// ..
	USBCLASS_Physical = 0x05,
	USBCLASS_Image,
	USBCLASS_Printer,
	USBCLASS_Mass_Storage,
	USBCLASS_HUB,
	USBCLASS_CDC_Data,
	USBCLASS_Smart_Card,
	// ..
	USBCLASS_Content_Security = 0x0D,
	USBCLASS_Video,
	USBCLASS_Personal_Healthcase,
	USBCLASS_AV_Device,
	USBCLASS_Billboard_Device,
	// ..
	USBCLASS_Diagnostic_Device = 0xDC,
	// ..
	USBCLASS_Wireless_Controller = 0xE0,
	// ..
	USBCLASS_Miscellaneous = 0xEF,
	// ..
	USBCLASS_Application_Specific = 0xFE,
	USBCLASS_Vendor_Specific
};

/***************************************************************************/

enum USBSPEED
{
	USBSPEED_Low = 0,
	USBSPEED_Full,
	USBSPEED_High,
	USBSPEED_Super,
};

/***************************************************************************/

// Standard Request Codes

enum
{
	REQCODE_Get_Status = 0,
	REQCODE_Clear_Feature,
	REQCODE_Reserved,
	REQCODE_Set_Feature,
	REQCODE_Reserved2,
	REQCODE_Set_Address,
	REQCODE_Get_Descriptor,
	REQCODE_Set_Descriptor,
	REQCODE_Get_Configuration,
	REQCODE_Set_Configuration,
	REQCODE_Get_Interface,
	REQCODE_Set_Interface,
	REQCODE_Synch_Frame,
};

// Descriptor Types

enum
{
	DSCTYPE_Device = 1,
	DSCTYPE_Configuration,
	DSCTYPE_String,
	DSCTYPE_Interface,
	DSCTYPE_EndPoint,
	DSCTYPE_Device_Qualifier,
	DSCTYPE_Other_Speed_Configuration,
	DSCTYPE_Interface_Power,
	DSCTYPE_OTG,
	DSCTYPE_Debug,
	DSCTYPE_Interface_Association,
};

/***************************************************************************/

struct USB2_Descriptor
{
	U8		Length;
	U8		DescriptorType;
};

/***************************************************************************/

struct USB2_SetupData
{
	U8		RequestType;
	U8		RequestCode;
	U16		Value;
	U16		Index;
	U16		Length;
};

#define REQTYPE_Host_to_Device	0x00
#define REQTYPE_Device_to_Host	0x80
#define REQTYPE_Write			REQTYPE_Host_to_Device
#define REQTYPE_Read			REQTYPE_Device_to_Host

#define REQTYPE_Standard		0x00
#define REQTYPE_Class			0x20
#define REQTYPE_Vendor			0x40

#define REQTYPE_Device			0x00
#define REQTYPE_Interface		0x01
#define REQTYPE_EndPoint		0x02
#define REQTYPE_Other			0x03

/***************************************************************************/

struct USB2_Device_Desc
{
	U8		Length;
	U8		DescriptorType;
	U16		USBVers;
	U8		DeviceClass;
	U8		DeviceSubClass;
	U8		DeviceProtocol;
	U8		MaxPacketSize0;
	U16		VendorID;
	U16		DeviceID;
	U16		DeviceVers;
	U8		ManufacturerStrNr;
	U8		DeviceStrNr;
	U8		SerialStrNr;
	U8		NumConfigurations;
};

/***************************************************************************/

struct USB2_DeviceQualifier_Desc
{
	U8		Length;
	U8		DescriptorType;
	U16		USB;
	U8		DeviceClass;
	U8		DeviceSubClass;
	U8		DeviceProtocol;
	U8		MaxPacketSize0;
	U8		NumConfigurations;
	U8		Reserved;
};

/***************************************************************************/

struct USB2_Config_Desc
{
	U8		Length;
	U8		DescriptorType;
	U16		TotalLength;
	U8		NumInterfaces;
	U8		ConfigurationValue;
	U8		ConfigurationStrNr;
	U8		Attributes;
	U8		MaxPower;
};

/***************************************************************************/

struct USB2_OtherSpeed_Desc
{
	U8		Length;
	U8		DescriptorType;
	U16		TotalLength;
	U8		NumInterfaces;
	U8		ConfigurationValue;
	U8		Configuration;
	U8		Attributes;
	U8		MaxPower;
};

/***************************************************************************/

struct USB2_Interface_Desc
{
	U8		Length;
	U8		DescriptorType;
	U8		InterfaceNumber;
	U8		AlternateSetting;
	U8		NumEndPoints;
	U8		InterfaceClass;
	U8		InterfaceSubClass;
	U8		InterfaceProtocol;
	U8		InterfaceStrNr;
};

/***************************************************************************/

struct USB2_EndPoint_Desc
{
	U8		Length;
	U8		DescriptorType;
	U8		EndPointAddress;
	U8		Attributes;
	U16		MaxPacketSize;
	U8		Interval;
};

#define EPADR_EndPoint_Mask		0x0F

#define EPADR_Dir_Mask			0x80
#define EPADR_Dir_Out			0x00
#define EPADR_Dir_In			0x80

#define EPADR_Dir_Number(x)		( x & EPADR_EndPoint_Mask )

#define EPATT_Type_Mask			0x03
#define EPATT_Type_Control		0x00
#define EPATT_Type_Isochronous	0x01
#define EPATT_Type_Bulk			0x02
#define EPATT_Type_Interrupt	0x03

/***************************************************************************/

struct USB2_String_Desc
{
	U8			Length;
	U8			DescriptorType;
	U16			LangID[];
};

/***************************************************************************/

// A Descriptor can max be 255 bytes (we use 256 for alignment)
// the HUB descriptor ends with two flexiable
// fields that depend on number of Ports
// I just make one large of max size
struct USB2_HUB_Desc
{
	U8			Length;
	U8			DescriptorType;
	U8			NbrPorts;
	U16			HubCharacteristics;
	U8			PwrOn2PwrGood;			// Delay in 2ms units
	U8			HubContrCurrent;
	U8			Data[256-7];
};

struct USB2_HUB_SS_Desc
{
	U8			Length;
	U8			DescriptorType;
	U8			NbrPorts;
	U16			HubCharacteristics;
	U8			PwrOn2PwrGood;			// Delay in 2ms units
	U8			HubContrCurrent;
    U8			HubHdrDecLat;
	U16			HubDelay;
	U8			DeviceRemovable[32];	// Max 255 ports
};

/***************************************************************************/

struct USB2_HID_Desc
{
	U8			Length;
	U8			DescriptorType;

	U16			Version;
	U8			CountryCode;
	U8			NumDescriptors;

	struct
	{
		U8		Type;
		U16		Length;
	}			Descriptors[1];
};

/***************************************************************************/

struct USB2_Association_Desc
{
	U8			Length;
	U8			DescriptorType;

	U8			FirstInterface;
	U8			InterfaceCount;

	U8			FunctionClass;
	U8			FunctionSubClass;
	U8			FunctionProtocol;
	U8			Function;
};

/***************************************************************************/

struct USB2_IORequest
{
	/* Standard */

	struct Message			io_Message;
	struct Device *			io_Device;
	struct Unit *			io_Unit;
	U16						io_Command;
	U8						io_Flags;
	S8						io_Error;
	U32						io_Actual;
	U32						io_Length;
	PTR						io_Data;
	U32						io_Offset;

	/* USB */

	U32						io_SetupLength;
	struct USB2_SetupData *	io_SetupData;

	U32						io_UserValue;			// Not used by the stack
	PTR						io_UserData;			// Not used by the stack

	U32						io_TimeOut;				// 0 = No Timeout, 1 sec = 1.000.000

	U8						io_AddZeroPackets;
	U8						io_AllowShortPackets;
	U8						io_Pad[2];

	/* Private Data Follows */
};

enum USB2Error
{
	USB2Err_NoError					= 0,

	USB2Err_Host_Stall				= -10,
	USB2Err_Host_CRCError			= -11,
	USB2Err_Host_HostError			= -12,
	USB2Err_Host_Timeout			= -13,
	USB2Err_Host_Overflow			= -14,
	USB2Err_Host_RuntPacket			= -15,
	USB2Err_Host_UnknownError		= -16,
	USB2Err_Host_NoBandWidth		= -17,
	USB2Err_Host_IllegalSpeed		= -18,	// Internal Error
	USB2Err_Host_IllegalEPType		= -19,	// Internal Error

	USB2Err_Stack_DirectionError	= -20,
	USB2Err_Stack_UnknownCommand	= -21,
	USB2Err_Stack_NoMemory			= -22,
	USB2Err_Stack_InvalidStructure	= -23,
	USB2Err_Stack_FunctionNotFound	= -24,
	USB2Err_Stack_EndPointNotFound	= -25,

	USB2Err_Device_Detached			= -30,
	USB2Err_Device_OwnershipChange	= -31,	// Not an error
	USB2Err_Device_Nak				= -32,	// Not an error
};


/***************************************************************************/

struct USB2_Function
{
	// Stack Version 1.1

	USB2_ID						HCD_ID;			// ID of the Host Controller
	USB2_ID						FKT_ID;			// ID of the Device
};

struct USB2_Interface
{
	// Stack Version 1.1

	USB2_ID						FKT_ID;			// ID of the Device
	USB2_ID						IFC_ID;			// ID of the Interface
	U32							IFC_Num;		// Interface Number
};

struct USB2_EndPoint
{
	U32	Private;
};

struct USB2_FDriver
{
	U32	Private;
};

/***************************************************************************/

struct USB2_DriverMessage
{
	U16								VendorID;
	U16								DeviceID;
	U32								DriverQuirks;
	struct USB2IFace *				IUSB2;
	struct USB2_Function *			Function;
	struct USB2_Interface *			Interface;
	struct USB2_Descriptor *		ConfigDescriptors;
	struct USB2_Interface_Desc *	InterfaceDescriptor;
};

enum USB2_DriverStat
{
	// No error driver tells the stack to try next driver
	USB2_DStat_Unsupported,

	// General error during startup
//	USBStartup_BindingError,

	// The driver ended normaly
//	USBStartup_NormalShutdown,

	// The driver encountered one or more errors
	// and had to shutdown
//	USBStartup_EmerhencyShutdown,

	// Task being promoted to a Process
//	USBStartup_PromoteShutdown,
};

/***************************************************************************/

struct USB2_NotifyMessage
{
	struct Message		nm_Message;
	U16					nm_Type;
	U16					nm_Command;
	USB2_ID				nm_ID;

	union
	{
		struct
		{
			U32			nm_Type;
			U32			nm_Index;
		}				nm_HCD;

		struct
		{
			USB2_ID		nm_HCD_ID;
			U32			nm_PortNr;
			USB2_ID		nm_Parent_ID;
		}				nm_Function;

		struct
		{
			U32			nm_Config_Nr;
			USB2_ID		nm_Config_StringID;
			USB2_ID		nm_Function_ID;
			U32			nm_Interface_Nr;
			U32			nm_Interface_AltNr;
			USB2_ID		nm_Interface_StringID;
		}				nm_Interface;

		struct
		{
			PTR			nm_Data;
		}				nm_Promote;
	}					nm_Data;
};

enum USBNotifyTypes
{
	USBNT_Stack = 0,
	USBNT_HCD,
	USBNT_Function,
	USBNT_Interface,
	USBNT_Last
};

enum USBNotifyCommand
{
	USBNC_Adding = 0,
	USBNC_Updating,
	USBNC_Removing,
	USBNC_FullBooted,
	USBNC_Detached,
	USBNC_Promote,
	USBNC_Last
};

/***************************************************************************/

struct USB2_EPResource			// EndPoint Resource
{
	struct Node					Node;				// Node is not used by the USB Stack
	U16							NodePad;			// Unused Longword align
	struct USB2_EndPoint *		EndPoint;
	struct USB2_IORequest **	IORequests;
	struct MsgPort *			MsgPort;
	U32							MsgPortBit;
	PTR *						Buffers;
	U32							BufferSize;
	struct USB2_SetupData *		SetupData;
	U32							EndPointMax;		// EndPoint Max Packet size
};

/***************************************************************************/

struct USB2_Register
{
	// Stack Version 1.1

	struct MsgPort *			Stack_MsgPort;
	U32							Stack_MsgPortBit;

	struct USB2_EPResource *	Res_Control;

	USB2_ID						HCD_ID;			// ID of the Host Controller
	USB2_ID						FKT_ID;			// ID of the Device

	// Struct may gow for future version

};

/***************************************************************************/

struct USB2_DeviceStatus
{
	U16	Stat;
};

#define USBSTATUS_Device_SelfPowered	0x01
#define USBSTATUS_Device_RemoteWakeup	0x02

struct USB2_InterfaceStatus
{
	U16	Stat;
};

// No flags for USBSTATUS_Interface_

struct USB2_EndPointStatus
{
	U16	Stat;
};

#define USBSTATUS_EndPoint_Halt			0x01

/***************************************************************************/

#define USB2Tag_Dummy					( 0x84140000 )

#if 0
#define USB2Tag_Stack_StartupTime		( USB2Tag_Dummy + 0 )	 // v1.1  - struct TimeVal *

#define USB2Tag_Stack_Status			( USB2Tag_Dummy + 1 )	 // v1.1  - U32

enum
{
	USB2Tag_Stat_Starting = 0,
	USB2Tag_Stat_Stopped,
	USB2Tag_Stat_Running,
	USB2Tag_Stat_Stopping,
	USB2Tag_Stat_Restarting,
};

#define USB2Tag_HCD_Type				( USB2Tag_Dummy + 2 )	 // v1.1  - U32

#endif

enum USB2HCDType
{
	HCDTYPE_UHCI,		// PCI - USB 1.1
	HCDTYPE_OHCI,		// PCI - USB 1.1
	HCDTYPE_EHCI,		// PCI - USB 2.0
	HCDTYPE_XHCI,		// PCI - USB 3.x
	HCDTYPE_SAM440,		// Onboard OHCI controller
	HCDTYPE_SAM460,		// Onboard OHCI controller
	HCDTYPE_P5020,		// Onboard EHCI controller
	HCDTYPE_P5040,		// Onboard EHCI controller
	HCDTYPE_TABOR,		// Onboard EHCI controller
	HCDTYPE_Last
};

// -- + --
// -- Find Tags ( Function / Interface )

// -- FKT (v1.1), IFC (v1.1)
#define USB2Tag_Find_VendorID			( USB2Tag_Dummy + 1 )	 // v1.1  - U32

// -- FKT (v1.1), IFC (v1.1)
#define USB2Tag_Find_DeviceID			( USB2Tag_Dummy + 2 )	 // v1.1  - U32

// -- FKT (v1.1), IFC (v1.1)
#define USB2Tag_Find_SeeClaimed			( USB2Tag_Dummy + 3 )	 // v1.1  - S32

// -- FKT (v1.1), IFC (v1.1)
#define USB2Tag_Find_Index				( USB2Tag_Dummy + 4 )	 // v1.1  - U32

// -- IFC (v1.1), IFC (v1.1)
#define USB2Tag_Find_Class				( USB2Tag_Dummy + 6 )	 // v1.1  - U32

// -- IFC (v1.1), IFC (v1.1)
#define USB2Tag_Find_SubClass			( USB2Tag_Dummy + 7 )	 // v1.1  - U32

// -- + --
// -- Function Driver

#define USB2Tag_FDriver_Type			( USB2Tag_Dummy + 51 )	 // v1.1  - U32
#define USB2Val_FDriver_Device			0
#define USB2Val_FDriver_Function		1
#define USB2Val_FDriver_Interface		2

#define USB2Tag_FDriver_Title			( USB2Tag_Dummy + 52 )	 // v1.1  - STR

#define USB2Tag_FDriver_Class_Filename	( USB2Tag_Dummy + 53 )	 // v1.1  - STR

#define USB2Tag_FDriver_Driver_Filename	( USB2Tag_Dummy + 54 )	 // v1.1  - STR

#define USB2Tag_FDriver_Entry			( USB2Tag_Dummy + 55 )	 // v1.1  - PTR

#define USB2Tag_FDriver_Priority		( USB2Tag_Dummy + 56 )	 // v1.1  - U32

#define USB2Tag_FDriver_Class			( USB2Tag_Dummy + 57 )	 // v1.1  - U32

#define USB2Tag_FDriver_SubClass		( USB2Tag_Dummy + 58 )	 // v1.1  - U32

#define USB2Tag_FDriver_Protocol		( USB2Tag_Dummy + 59 )	 // v1.1  - U32

// -- + --
// -- Register

#define USB2Tag_Reg_DriverMessage		( USB2Tag_Dummy + 101 )	 // v1.1  - struct USB2_DriverMessage *

#define USB2Tag_Reg_Function			( USB2Tag_Dummy + 102 )	 // v1.1  - struct USB2_Function *

#define USB2Tag_Reg_Interface			( USB2Tag_Dummy + 103 )	 // v1.1  - struct USB2_Interface *

#define USB2Tag_Reg_Title				( USB2Tag_Dummy + 104 )	 // v1.1  - STR

#define USB2Tag_Reg_TimeOut				( USB2Tag_Dummy + 105 )	 // v1.1  - U32

// -- + --
// -- IORequest

#define USB2Tag_IOReq_TimeOut			( USB2Tag_Dummy + 151 )	 // v1.1  - U32

#define USB2Tag_IOReq_MsgPort			( USB2Tag_Dummy + 152 )	 // v1.1  - struct MsgPort *

#define USB2Tag_IOReq_EndPoint			( USB2Tag_Dummy + 153 )	 // v1.1  - struct USB2_EndPoint *

#define USB2Tag_IOReq_AddZeroPacket		( USB2Tag_Dummy + 154 )	 // v1.1  - U32 (BOOL)

#define USB2Tag_IOReq_AllowShortPackets	( USB2Tag_Dummy + 155 )	 // v1.1  - U32 (BOOL)


// -- + --
// -- EndPoint Resource

#define USB2Tag_EPRes_TimeOut			( USB2Tag_Dummy + 201 )	 // v1.1  - U32

#define USB2Tag_EPRes_MsgPort			( USB2Tag_Dummy + 202 )	 // v1.1  - struct MsgPort *

#define USB2Tag_EPRes_EPType			( USB2Tag_Dummy + 203 )	 // v1.1  - EPATT_Type_xxx

#define USB2Tag_EPRes_EPDirection		( USB2Tag_Dummy + 204 )	 // v1.1  - EPADR_Dir_xxx

#define USB2Tag_EPRes_EndPointNr		( USB2Tag_Dummy + 205 )	 // v1.1  - U32

#define USB2Tag_EPRes_InterfaceNr		( USB2Tag_Dummy + 206 )	 // v1.1  - U32

#define USB2Tag_EPRes_BufferSize		( USB2Tag_Dummy + 207 )	 // v1.1  - U32
#define USB2Val_BufferSize_MaxPacketSize		-1
#define USB2Val_BufferSize_MaxPacketSize_2		-2
#define USB2Val_BufferSize_MaxPacketSize_4		-3
#define USB2Val_BufferSize_MaxPacketSize_8		-4
#define USB2Val_BufferSize_MaxPacketSize_16		-5
#define USB2Val_BufferSize_MaxPacketSize_32		-6
#define USB2Val_BufferSize_MaxPacketSize_64		-7
#define USB2Val_BufferSize_MaxPacketSize_128	-8
#define USB2Val_BufferSize_MaxPacketSize_256	-9

#define USB2Tag_EPRes_NrOfIORequest		( USB2Tag_Dummy + 208 )	 // v1.1  - U32

#define USB2Tag_EPRes_AddZeroPacket		( USB2Tag_Dummy + 209 )	 // v1.1  - U32 (BOOL)

#define USB2Tag_EPRes_AllowShortPackets	( USB2Tag_Dummy + 210 )	 // v1.1  - U32 (BOOL)


// -- + --
// -- Attribute

#define USB2Tag_Attr_DeviceSize			( USB2Tag_Dummy + 251 )	 // v1.1  - [G.] U32
#define USB2Tag_Attr_DeviceDesc			( USB2Tag_Dummy + 252 )	 // v1.1  - [G.] struct USB2_Device_Desc

#define USB2Tag_Attr_HUBSize			( USB2Tag_Dummy + 253 )	 // v1.1  - [G.] U32
#define USB2Tag_Attr_HUBDesc			( USB2Tag_Dummy + 254 )	 // v1.1  - [G.] struct USB2_HUB_Desc

#define USB2Tag_Attr_ConfigSize			( USB2Tag_Dummy + 255 )	 // v1.1  - [G.] U32
#define USB2Tag_Attr_ConfigDesc			( USB2Tag_Dummy + 256 )	 // v1.1  - [G.] struct USB2_Config_Desc




// -- + --
// --



#if 0
#define USB2Tag_HCD_Index				( USB2Tag_Dummy + 3 )	 // v1.1  - U32

#define USB2Tag_FKT_DeviceDescriptor	( USB2Tag_Dummy + 4 )	 // v1.1  - struct USB2_Device_Desc

#define USB2Tag_FKT_HUBDescriptor		( USB2Tag_Dummy + 5 )	 // v1.1  - struct USB2_HUB_Desc

#define USB2Tag_FKT_ConfigDescriptor	( USB2Tag_Dummy + 6 )	 // v1.1  - struct USB2_Config_Desc + [Data]

#define USB2Tag_FKT_ConfigDescriptorSize ( USB2Tag_Dummy + 7 )	  // v1.1  - U32 *
#endif






#if 0
#define USB2Tag_ShortPackets			( USB2Tag_Dummy + 17 )	 // v1.1  - U32

#define USB2Tag_SetupBufferSize			( USB2Tag_Dummy + 18 )	 // v1.1  - U32
#endif

#if 0
#define USB2Tag_Stat_Buffer_Name		( USB2Tag_Dummy + 21 )	 // v1.1  - STR *

#define USB2Tag_Stat_Buffer_Total		( USB2Tag_Dummy + 22 )	 // v1.1  - U32 *

#define USB2Tag_Stat_Buffer_Used		( USB2Tag_Dummy + 23 )	 // v1.1  - U32 *

#define USB2Tag_Stat_Buffer_Free		( USB2Tag_Dummy + 24 )	 // v1.1  - U32 *

#define USB2Tag_Stat_Buffer_MaxFree		( USB2Tag_Dummy + 25 )	 // v1.1  - U32 *

#define USB2Tag_Stat_Buffer_Alignment	( USB2Tag_Dummy + 26 )	 // v1.1  - U32 *

#define USB2Tag_Stat_Buffer_Size		( USB2Tag_Dummy + 27 )	 // v1.1  - U32 *

#define USB2Tag_Stat_Transfer_Control	( USB2Tag_Dummy + 28 )	 // v1.1  - U64 *

#define USB2Tag_Stat_Transfer_Interrupt	( USB2Tag_Dummy + 29 )	 // v1.1  - U64 *

#define USB2Tag_Stat_Transfer_Bulk		( USB2Tag_Dummy + 30 )	 // v1.1  - U64 *

#define USB2Tag_Stat_Transfer_Isochonous ( USB2Tag_Dummy + 31 )  // v1.1  - U64 *

#define USB2Tag_Stat_Transfer_Total		( USB2Tag_Dummy + 32 )	 // v1.1  - U64 *

//#define USB2Tag_Reg_DeviceList		   ( USB2Tag_Dummy + 33 )   // v1.1  - PTR

//#define USB2Tag_Reg_List_Entry_Size	   ( USB2Tag_Dummy + 34 )   // v1.1  - U32

//#define USB2Tag_Reg_List_VendorID_Offset ( USB2Tag_Dummy + 35 )   // v1.1  - U32

//#define USB2Tag_Reg_List_DeviceID_Offset ( USB2Tag_Dummy + 36 )  // v1.1  - U32
#endif

#if 0
#define USB2Tag_FDriver_Promote			( USB2Tag_Dummy + 44 )	 // v1.1  - PTR
#endif

#if 0
#define USB2Tag_FDriver_Stack			( USB2Tag_Dummy + 49 )	 // v1.1  - U32

#define USB2Tag_FDriver_DeviceList		( USB2Tag_Dummy + 50 )	 // v1.1  - struct List *

#define USB2Tag_FDriver_DriverDate		( USB2Tag_Dummy + 51 )	 // v1.1  - struct DateStamp *

#define USB2Tag_FDriver_Hotkey			( USB2Tag_Dummy + 52 )	 // v1.1  - STR
#endif


#if 0
#define USB2Tag_FKT_ManufacturerStr		( USB2Tag_Dummy + 54 )	 // v1.4  - U16 *

#define USB2Tag_FKT_DeviceStr			( USB2Tag_Dummy + 55 )	 // v1.4  - U16 *

#define USB2Tag_FKT_SerialStr			( USB2Tag_Dummy + 56 )	 // v1.4  - U16 *

#define USB2Tag_CFG_String				( USB2Tag_Dummy + 57 )	 // v1.6  - U16 *

#define USB2Tag_IFC_String				( USB2Tag_Dummy + 58 )	 // v1.6  - U16 *
#endif



struct USB2_DeviceNode
{
	struct Node			Node;
	U16					VendorID;
	U16					DeviceID;
	U32					Quirk;
};

/***************************************************************************/

#pragma pack()

#endif /* DEVICES_USB3_H */
