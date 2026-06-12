
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "All.h"

// --





#if 0

/*
 * Universal serial bus attempt by Rene W. Olsen
 *
 * Copyright (c) 2007-2015 by Rene W. Olsen < renewolsen @ gmail . com >
 * All rights reserved.
 *
 */

// --

#include "Debug.h"
#include "Driver.h"
#include "usbdevs.h"

// --

// --s

// --

static struct ethBase *		Device_Init(					struct ethBase *ethBase, APTR seglist, struct ExecIFace *myIExec );
static U32 				Device_Obtain(					struct DeviceManagerInterface *Self );
static U32				Device_Release(					struct DeviceManagerInterface *Self );
static S32				Device_Open(					struct DeviceManagerInterface *Self, struct IOSana2Req *ioreq, U32 unitnum, U32 flags );
static APTR					Device_Close(					struct DeviceManagerInterface *Self, struct IOSana2Req *ioreq );
static APTR					Device_Expunge(					struct DeviceManagerInterface *Self );
static void					Device_BeginIO(					struct DeviceManagerInterface *Self, struct IOSana2Req *ioreq );
static void					Device_AbortIO(					struct DeviceManagerInterface *Self, struct IOSana2Req *ioreq );

// --

static void 				Cmd_Read(						struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				Cmd_Write(						struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				Cmd_Flush(						struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				Cmd_Flush(						struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				Cmd_Invalid(					struct devUnit *unit, struct IOSana2Req *ioreq );

static void 				NSCmd_DeviceQuery(				struct devUnit *unit, struct IOStdReq *ioreq );

static void 				s2_Sana2Hook(					struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_DeviceQuery(					struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_GetStationAddress(			struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_ConfigInterface(				struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_AddMultiCastAddress(			struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_DelMultiCastAddress(			struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_MultiCast(					struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_BroadCast(					struct devUnit *unit, struct IOSana2Req *ioreq );
//static void				  s2_TrackType(					  struct devUnit *unit, struct IOSana2Req *ioreq );
//static void				  s2_UntrackType(				  struct devUnit *unit, struct IOSana2Req *ioreq );
//static void				  s2_GetTypeStats(				  struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_GetSpecialStats(				struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_GetGlobalStats(				struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_OnEvent(						struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_ReadOrphan(					struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_Online(						struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_Offline(						struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_GetExtendedGlobalStats(		struct devUnit *unit, struct IOSana2Req *ioreq );
static void 				s2_Sample_ThroughPut(			struct devUnit *unit, struct IOSana2Req *ioreq );

// --

static int 				ADM8511_Init(					struct devUnit *unit );
static int					ADM8511_Get_EEPromRegister(		struct devUnit *unit, uint8 *buf, U32 location );
static int					ADM8511_Get_PhyRegister(		struct devUnit *unit, uint16 *buf, U32 id, U32 offset );
static int					ADM8511_Set_PhyRegister(		struct devUnit *unit, U32 value, U32 id, U32 offset );
static int					ADM8511_PhyAutoNegotiate(		struct devUnit *unit );

// --

static int					myOpenResources(				struct ethBase *ethBase );
static void					myCloseResources(				struct ethBase *ethBase );

static int					task_Begin(						struct ethBase *ethBase, struct devUnit *unit );
static void					task_Finish(					struct ethBase *ethBase, struct devUnit *unit );

static void					task_Entry(						void );

static int					task_Setup(						struct devUnit *unit );
static void					task_Cleanup(					struct devUnit *unit );

static void					task_FreeLists(					struct devUnit *unit );

static void					task_GetConfiguration(			struct devUnit *unit );

static int					task_InitMsgPorts(				struct devUnit *unit );
static void 				task_FreeMsgPorts(				struct devUnit *unit );

// --

static void					CalcMulticastBits(				struct devUnit *unit );
static struct MulticastAddress *FindMulticastAddress(		struct devUnit *unit, uint8 *address );
static struct MulticastAddress *FindMulticastAddressViaBit(	struct devUnit *unit, U32 bit_number );

static U32				CalculateCRC(					uint8 *addr );

static void 				CheckEvent(						struct devUnit *unit, U32 events );

static int					AbortAll(						struct devUnit *unit, struct BufferManagement *bm, U32 wire_error );
static S32				AbortReqOnList(					struct devUnit *unit, struct List *list, struct IOSana2Req *abort_ioreq, U32 wire_error );
static S32				AbortReqOnListBackPtr(			struct devUnit *unit, struct List *list, struct BufferManagement *bm, U32 wire_error );

// --

// Supported Ethernet Devices, thats known to work with the ADM8511 chip
struct USB_Ethernet_Device USB_Device_IDs[] =
{
	{ USB_VENDOR_3COM,			USB_PRODUCT_3COM_3C460B,			PII },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX1,				PNA|PII },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX2,				PII },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_UFE1000,			LSYS },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX4,				PNA },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX5,				PNA },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX6,				PII },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX7,				PII },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX8,				PII },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX9,				PNA },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_XX10,			0 },
	{ USB_VENDOR_ABOCOM,		USB_PRODUCT_ABOCOM_DSB650TX_PNA,	0 },
	{ USB_VENDOR_ACCTON,		USB_PRODUCT_ACCTON_USB320_EC,		0 },
	{ USB_VENDOR_ACCTON,		USB_PRODUCT_ACCTON_SS1001,			PII },
	{ USB_VENDOR_ADMTEK,		USB_PRODUCT_ADMTEK_PEGASUS,			PNA },
	{ USB_VENDOR_ADMTEK,		USB_PRODUCT_ADMTEK_PEGASUSII,		PII },
	{ USB_VENDOR_ADMTEK,		USB_PRODUCT_ADMTEK_PEGASUSII_2,		PII },
	{ USB_VENDOR_ADMTEK,		USB_PRODUCT_ADMTEK_PEGASUSII_3,		PII },
	{ USB_VENDOR_ADMTEK,		USB_PRODUCT_ADMTEK_PEGASUSII_4,		PII },
	{ USB_VENDOR_AEI,			USB_PRODUCT_AEI_FASTETHERNET,		PII },
	{ USB_VENDOR_ALLIEDTELESYN, USB_PRODUCT_ALLIEDTELESYN_ATUSB100,	PII },
	{ USB_VENDOR_ATEN,			USB_PRODUCT_ATEN_UC110T,			PII },
	{ USB_VENDOR_BELKIN,		USB_PRODUCT_BELKIN_F5D5050,			PII },
	{ USB_VENDOR_BILLIONTON,	USB_PRODUCT_BILLIONTON_USB100,		0 },
	{ USB_VENDOR_BILLIONTON,	USB_PRODUCT_BILLIONTON_USBLP100,	PNA },
	{ USB_VENDOR_BILLIONTON,	USB_PRODUCT_BILLIONTON_USBEL100,	0 },
	{ USB_VENDOR_BILLIONTON,	USB_PRODUCT_BILLIONTON_USBE100,		PII },
	{ USB_VENDOR_COREGA,		USB_PRODUCT_COREGA_FETHER_USB_TX,	0 },
	{ USB_VENDOR_COREGA,		USB_PRODUCT_COREGA_FETHER_USB_TXS,	PII },
	{ USB_VENDOR_DLINK,			USB_PRODUCT_DLINK_DSB650TX4,		LSYS|PII },
	{ USB_VENDOR_DLINK,			USB_PRODUCT_DLINK_DSB650TX1,		LSYS },
	{ USB_VENDOR_DLINK,			USB_PRODUCT_DLINK_DSB650TX,			LSYS },
	{ USB_VENDOR_DLINK,			USB_PRODUCT_DLINK_DSB650TX_PNA,		PNA },
	{ USB_VENDOR_DLINK,			USB_PRODUCT_DLINK_DSB650TX3,		LSYS|PII },
	{ USB_VENDOR_DLINK,			USB_PRODUCT_DLINK_DSB650TX2,		LSYS|PII },
	{ USB_VENDOR_DLINK,			USB_PRODUCT_DLINK_DSB650,			0 },
	{ USB_VENDOR_ELCON,			USB_PRODUCT_ELCON_PLAN,				PNA|PII },
	{ USB_VENDOR_ELECOM,		USB_PRODUCT_ELECOM_LDUSB30,			PII },
	{ USB_VENDOR_ELECOM,		USB_PRODUCT_ELECOM_LDUSBTX0,		0 },
	{ USB_VENDOR_ELECOM,		USB_PRODUCT_ELECOM_LDUSBTX1,		LSYS },
	{ USB_VENDOR_ELECOM,		USB_PRODUCT_ELECOM_LDUSBTX2,		0 },
	{ USB_VENDOR_ELECOM,		USB_PRODUCT_ELECOM_LDUSBTX3,		LSYS },
	{ USB_VENDOR_ELECOM,		USB_PRODUCT_ELECOM_LDUSBLTX,		PII },
	{ USB_VENDOR_ELSA,			USB_PRODUCT_ELSA_USB3ETHERNET,		0 },
	{ USB_VENDOR_GIGABYTE,		USB_PRODUCT_GIGABYTE_GNBR402W,		0 },
	{ USB_VENDOR_HAWKING,		USB_PRODUCT_HAWKING_UF100,			PII },
	{ USB_VENDOR_HP,			USB_PRODUCT_HP_HN210E,				PII },
	{ USB_VENDOR_IODATA,		USB_PRODUCT_IODATA_USBETTX,			0 },
	{ USB_VENDOR_IODATA,		USB_PRODUCT_IODATA_USBETTXS,		PII },
	{ USB_VENDOR_KINGSTON,		USB_PRODUCT_KINGSTON_KNU101TX,		0 },
	{ USB_VENDOR_LINKSYS,		USB_PRODUCT_LINKSYS_USB10TX1,		LSYS|PII },
	{ USB_VENDOR_LINKSYS,		USB_PRODUCT_LINKSYS_USB10T,			LSYS },
	{ USB_VENDOR_LINKSYS,		USB_PRODUCT_LINKSYS_USB100TX,		LSYS },
	{ USB_VENDOR_LINKSYS,		USB_PRODUCT_LINKSYS_USB100H1,		LSYS|PNA },
	{ USB_VENDOR_LINKSYS,		USB_PRODUCT_LINKSYS_USB10TA,		LSYS },
	{ USB_VENDOR_LINKSYS,		USB_PRODUCT_LINKSYS_USB10TX2,		LSYS|PII },
	{ USB_VENDOR_MICROSOFT,		USB_PRODUCT_MICROSOFT_MN110,		PII },
	{ USB_VENDOR_MELCO, 		USB_PRODUCT_MELCO_LUATX1,			0 },
	{ USB_VENDOR_MELCO, 		USB_PRODUCT_MELCO_LUATX5,			0 },
	{ USB_VENDOR_MELCO, 		USB_PRODUCT_MELCO_LUA2TX5,			PII },
	{ USB_VENDOR_MOBILITY,		USB_PRODUCT_MOBILITY_EASIDOCK,		0 },
	{ USB_VENDOR_NETGEAR,		USB_PRODUCT_NETGEAR_FA101,			PII },
	{ USB_VENDOR_OCT,			USB_PRODUCT_OCT_USBTOETHER,			PII },
	{ USB_VENDOR_SIEMENS,		USB_PRODUCT_SIEMENS_SPEEDSTREAM,	PII },
	{ USB_VENDOR_SMARTBRIDGES,	USB_PRODUCT_SMARTBRIDGES_SMARTNIC,	PII },
	{ USB_VENDOR_SMC,			USB_PRODUCT_SMC_2202USB,			0 },
	{ USB_VENDOR_SMC,			USB_PRODUCT_SMC_2206USB,			PII },
	{ USB_VENDOR_SOHOWARE,		USB_PRODUCT_SOHOWARE_NUB100,		0 },
	{ USB_VENDOR_SOHOWARE,		USB_PRODUCT_SOHOWARE_NUB110,		PII },
	{ USB_VENDOR_LOGITEC,		USB_PRODUCT_LOGITEC_LANTX,   		PII },

	// -Amiga- My own confirmed Devices that work

	// Philips (0x083a)
	{ 0x083a, 0xb004, PII },

	// The End
	{ 0x0000, 0x0000, 0 }
};

static CONST_APTR _manager_Vectors[] =
{
	(APTR) Device_Obtain,
	(APTR) Device_Release,
    (APTR) NULL,
    (APTR) NULL,
	(APTR) Device_Open,
	(APTR) Device_Close,
	(APTR) Device_Expunge,
    (APTR) NULL,
	(APTR) Device_BeginIO,
	(APTR) Device_AbortIO,
	(APTR) -1
};

static struct TagItem _manager_Tags[] =
{
	{ MIT_Name,			(Tag) "__device"		},
	{ MIT_VectorTable,	(Tag) _manager_Vectors	},
	{ MIT_Version,		(Tag) 1					},
	{ TAG_END,			(Tag) 0					}
};

static CONST_APTR devInterfaces[] =
{
	(APTR) _manager_Tags,
	(APTR) NULL
};

static struct TagItem devCreateTags[] =
{
	{ CLT_DataSize,		(Tag) sizeof( struct ethBase )	},
	{ CLT_Interfaces,	(Tag) devInterfaces				},
	{ CLT_InitFunc,		(Tag) Device_Init				},
	{ TAG_END,			(Tag) 0							}
};

static USED struct Resident eth_res =
{
/* rt_MatchWord */						RTC_MATCHWORD,
/* rt_MatchTag  */	(struct Resident *)	( & eth_res + 0 ),
/* rt_EndSkip   */	(APTR)				( & eth_res + 1 ),
/* rt_Flags     */						RTF_NATIVE|RTF_AUTOINIT|RTF_COLDSTART,
/* rt_Version   */						VERSION,
/* rt_Type      */						NT_DEVICE,
/* rt_Pri       */						0,
/* rt_Name      */	(STR)			DEVNAME,
/* rt_IdString  */	(STR)			VSTRING,
/* rt_Init      */	(APTR)				devCreateTags
};

// --

static TEXT USED	verstag[]		= VERSTAG "(c) Amiga Kit";

struct Library *		NewlibBase		= NULL;
struct Interface *		INewlib			= NULL;

struct ExecIFace *		IExec			= NULL;

struct Library *		DOSBase			= NULL;
struct DOSIFace *		IDOS			= NULL;

struct Library *		UtilityBase		= NULL;
struct UtilityIFace *	IUtility		= NULL;

struct TimerIFace *		ITimer			= NULL;

///

/* -- */

/// _start

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS4) this was usually done by
 * moveq #0,d0
 * rts
 *
 */

S32 _start( STR argstring UNUSED, S32 arglen UNUSED, APTR SysBase UNUSED )
{
    /* If you feel like it, open DOS and print something to the user */

    return( RETURN_OK );
}

///

/* -- Device Interface -- */

/// Device_Init

static struct ethBase *Device_Init( struct ethBase *ethBase, APTR seglist, struct ExecIFace *myIExec )
{
struct ethBase *retval;

	IExec	= myIExec;

	ethBase->eth_Base.lib_Node.ln_Type	= NT_DEVICE;
	ethBase->eth_Base.lib_Node.ln_Name	= (STR) DEVNAME;
	ethBase->eth_Base.lib_Flags			= LIBF_SUMUSED|LIBF_CHANGED;
	ethBase->eth_Base.lib_Version		= VERSION;
	ethBase->eth_Base.lib_IdString		= (STR) VSTRING;
	ethBase->eth_Base.lib_Revision		= REVISION;
	ethBase->eth_SegmentList			= seglist;

	if ( myOpenResources( ethBase ))
    {
		retval = ethBase;
    }
    else
    {
		myCloseResources( ethBase );

		retval = NULL;
    }
	return(		retval );
}

///
/// Device_Obtain

static U32 Device_Obtain( struct DeviceManagerInterface *Self )
{
    Self->Data.RefCount++;

    return( Self->Data.RefCount );
}

static U32 Device_Release( struct DeviceManagerInterface *Self )
{
    Self->Data.RefCount--;

    return( Self->Data.RefCount );
}

///
/// Device_Open

static S32 Device_Open(
    struct DeviceManagerInterface *Self,
    struct IOSana2Req *ioreq,
    U32 unitnum,
	U32 flags UNUSED )
{
struct BufferManagement *bm;
struct TagItem *taglist;
struct ethBase *ethBase;
struct devUnit *unit;
S32 error;

	ethBase = (APTR) Self->Data.LibBase;

	IExec->ObtainSemaphore( & ethBase->eth_Semaphore );

    /* Expunge protection */
	ethBase->eth_Base.lib_OpenCnt++;

    /* Check request size */
    if ( ioreq->ios2_Req.io_Message.mn_Length < sizeof( struct IOSana2Req ))
    {
	    SHOWMSG( "Invalid request size" );
        error = IOERR_OPENFAIL;
        goto bailout;
    }

    /* Check for valid Unit number (must be 0-31)*/
    if ( unitnum > 31 )
    {
	    SHOWMSG( "Invalid unit number" );
        error = IOERR_OPENFAIL;
        goto bailout;
    }

	// Make sure that before we fill in the goods, the tag item list is
	// actually present we'll be looking at.
    if ( ioreq->ios2_Req.io_Message.mn_Length >= sizeof( struct IOSana2Req ))
    {
		taglist = (APTR) ioreq->ios2_BufferManagement;

    	// Allocate the buffer management info before using the unit
		bm = IExec->AllocVecTags( sizeof( struct BufferManagement ),
		    AVT_Clear, 0,
		    TAG_END
	    );

		if ( bm == NULL )
        {
		    SHOWMSG( "Error allocating memory" );
	        error = S2ERR_NO_RESOURCES;
	        goto bailout;
        }

    	// 'Dummy' SANA2 callback defaults are provided to be able to work even without
    	// buffer management routines.  This is for use by status type tools.

		bm->bm_CopyToBuff	    = (APTR) IUtility->GetTagData( S2_CopyToBuff,   0, taglist );
		bm->bm_CopyFromBuff	    = (APTR) IUtility->GetTagData( S2_CopyFromBuff, 0, taglist );
		bm->bm_PacketHook	    = (APTR) IUtility->GetTagData( S2_PacketFilter, 0, taglist );
		bm->bm_CopyToBuffTag    = (Tag) NULL;
		bm->bm_CopyFromBuffTag	= (Tag) NULL;

		ioreq->ios2_BufferManagement = (APTR) bm;
    }

	unit = ethBase->eth_Units[ unitnum ];

	if ( unit )
	{
		goto alreadyopen;
	}

	/* Create per unit data */
	unit = IExec->AllocVecTags( sizeof( struct devUnit ),
		AVT_Clear, 0,
		TAG_END
	);

	if ( unit == NULL )
	{
		error = IOERR_OPENFAIL;
		goto bailout;
	}

	unit->unit_ethBase		= ethBase;
	unit->unit_UnitNumber	= unitnum;

	/* Start the subtask */

    if ( task_Begin( ethBase, unit ) == FALSE )
	{
		IExec->FreeVec( unit );
		error = IOERR_OPENFAIL;
		goto bailout;
	}

	ethBase->eth_Units[ unitnum	] = unit;

alreadyopen:

	ioreq->ios2_Req.io_Unit = (APTR) unit;
	unit->unit_Unit.unit_OpenCnt++;

	/* Prevent delayed expunges */
	ethBase->eth_Base.lib_Flags &= ~LIBF_DELEXP;

	/* Mark IORequest as "complete" */
	ioreq->ios2_Req.io_Message.mn_Node.ln_Type = NT_REPLYMSG;

	/* Success */
	error = 0;

bailout:

    if ( error == 0 )
    {
		ethBase->eth_Base.lib_OpenCnt++;
    }
    else
    {
        ioreq->ios2_Req.io_Device = NULL;
    }

    ioreq->ios2_Req.io_Error = error;

    /* End Expunge protection */
	ethBase->eth_Base.lib_OpenCnt--;

	IExec->ReleaseSemaphore( & ethBase->eth_Semaphore );

    return( error );
}

///
/// Device_Close

static APTR Device_Close( struct DeviceManagerInterface *Self, struct IOSana2Req *ioreq )
{
struct ethBase *ethBase;
struct devUnit *unit;
APTR seglist;

	seglist = NULL;

	ethBase = (APTR) Self->Data.LibBase;

	IExec->ObtainSemaphore( & ethBase->eth_Semaphore );

	ethBase->eth_Base.lib_OpenCnt--;

	// -- Free IORequest Resources

	unit = (APTR) ioreq->ios2_Req.io_Unit;

	ioreq->ios2_Req.io_Unit	= (APTR)-1;
    ioreq->ios2_Req.io_Device = (APTR)-1;

    if (( ioreq->ios2_Req.io_Message.mn_Length >= sizeof(*ioreq)
    &&  ( ioreq->ios2_BufferManagement != NULL )))
    {
	    IExec->FreeVec( ioreq->ios2_BufferManagement );
        ioreq->ios2_BufferManagement = NULL;
    }

	// -- Check if we should Stop Unit Task

	unit->unit_Unit.unit_OpenCnt--;

	if ( unit->unit_Unit.unit_OpenCnt )
    {
		goto bailout;
	}

	task_Finish( ethBase, unit );

	ethBase->eth_Units[ unit->unit_UnitNumber ] = NULL;

	IExec->FreeVec( unit );

	// -- Check if we should Expunge

	#ifdef SIRION
	if ( ethBase->eth_Base.lib_OpenCnt == 0 )
	{
		if ( ethBase->eth_Base.lib_Flags & LIBF_DELEXP )
	    {
			seglist = Device_Expunge( Self );
	    }
	}
	#endif

	// -- All done

bailout:

	IExec->ReleaseSemaphore( & ethBase->eth_Semaphore );

	return( seglist );
}

///
/// Device_Expunge

static APTR Device_Expunge( struct DeviceManagerInterface *Self UNUSED )
{
struct ethBase *ethBase;
APTR seglist;

	ethBase = (APTR) Self->Data.LibBase;

	if ( ethBase->eth_Base.lib_OpenCnt == 0 )
    {
		seglist = ethBase->eth_SegmentList;

		myCloseResources( ethBase );

        /* Remove Device from Public */

		IExec->Remove(			(APTR) ethBase );
		IExec->DeleteLibrary(	(APTR) ethBase );
    }
    else
    {
		ethBase->eth_Base.lib_Flags |= LIBF_DELEXP;
		
		seglist = NULL;
    }

	return( seglist );
}

///
/// Device_BeginIO

static void Device_BeginIO( struct DeviceManagerInterface *Self UNUSED, struct IOSana2Req *ioreq )
{
struct devUnit *unit;


	unit	= (APTR) ioreq->ios2_Req.io_Unit;

	// Disable QUICKIO - we cannot support that
	ioreq->ios2_Req.io_Flags &= ~IOF_QUICK;
	ioreq->ios2_Req.io_Error = 0;

	switch( ioreq->ios2_Req.io_Command )
	{
		case CMD_READ:
		case CMD_WRITE:
		case CMD_FLUSH:
		case CMD_INVALID:
		case NSCMD_DEVICEQUERY:
		case S2_DEVICEQUERY:
		case S2_GETSTATIONADDRESS:
		case S2_CONFIGINTERFACE:
		case S2_SANA2HOOK:
	    case S2_ADDMULTICASTADDRESS:
	    case S2_DELMULTICASTADDRESS:
		case S2_MULTICAST:
		case S2_BROADCAST:
		case S2_TRACKTYPE:
		case S2_UNTRACKTYPE:
		case S2_GETTYPESTATS:
		case S2_GETSPECIALSTATS:
		case S2_GETGLOBALSTATS:
		case S2_ONEVENT:
		case S2_READORPHAN:
		case S2_ONLINE:
		case S2_OFFLINE:
		case S2_GETEXTENDEDGLOBALSTATS:
		case S2_SAMPLE_THROUGHPUT:
		{
			if ( unit->unit_BeginIOMsgPort )
			{
				IExec->PutMsg( unit->unit_BeginIOMsgPort, &ioreq->ios2_Req.io_Message );
			}
			else
			{
				ioreq->ios2_Req.io_Error = IOERR_UNITBUSY;

				IExec->ReplyMsg( (APTR) ioreq );
			}
			break;
		}

		default:
		{
			// Unknown Command
			ioreq->ios2_Req.io_Error = IOERR_NOCMD;

			IExec->ReplyMsg( (APTR) ioreq );
			break;
		}
	}

}

///
/// Device_AbortIO

//  DevAbortIO() filters any request that doesn't need to be aborted and
//  passes the requests that need to be aborted to the device task

static void Device_AbortIO( struct DeviceManagerInterface *Self UNUSED, struct IOSana2Req *ioreq )
{
struct AbortIOMessage *msg;
struct devUnit *unit;


	unit    = (APTR) ioreq->ios2_Req.io_Unit;

	// Only the required commands are aborted
    switch( ioreq->ios2_Req.io_Command )
	{
		case CMD_READ:
		case CMD_WRITE:
		case S2_READORPHAN:
		case S2_BROADCAST:
		case S2_MULTICAST:
		case S2_ONEVENT:
		case S2_SAMPLE_THROUGHPUT:
		{
			if ( unit->unit_AbortIOMsgPort )
			{
				msg = IExec->AllocVecTags( sizeof( struct AbortIOMessage ),
					AVT_Clear, 0,
					TAG_END
				);

				if ( msg )
				{
					msg->ioreq = ioreq;

					// Forward request to device task
					IExec->PutMsg( unit->unit_AbortIOMsgPort, (APTR) msg );
				}
				else
				{
					ioreq->ios2_Req.io_Error = IOERR_UNITBUSY;

					IExec->ReplyMsg( (APTR) ioreq );
				}
			}
			else
			{
				ioreq->ios2_Req.io_Error = IOERR_UNITBUSY;

				IExec->ReplyMsg( (APTR) ioreq );
			}
			break;
		}

		default:
		{
			ioreq->ios2_Req.io_Error = IOERR_NOCMD;

			IExec->ReplyMsg( (APTR) ioreq );
			break;
		}
	}

}

///

/* -- Sana2 Commands -- */

/// 0000 Cmd_Invalid

static void Cmd_Invalid( struct devUnit *unit UNUSED, struct IOSana2Req *ioreq )
{
    ioreq->ios2_Req.io_Error = IOERR_NOCMD;

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 0002 Cmd_Read

static void Cmd_Read( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	if ( unit->unit_Detached )
	{
		SHOWMSG( "USB Device Detached" );

		ioreq->ios2_Req.io_Error	= S2ERR_OUTOFSERVICE;
		ioreq->ios2_WireError		= S2WERR_UNIT_OFFLINE;

		IExec->ReplyMsg( (APTR) ioreq );
		goto bailout;
	}

	if ( unit->unit_Online == FALSE )
	{
		SHOWMSG( "USB Device is Offline" );

		ioreq->ios2_Req.io_Error	= S2ERR_OUTOFSERVICE;
		ioreq->ios2_WireError		= S2WERR_UNIT_OFFLINE;

		IExec->ReplyMsg( (APTR) ioreq );
		goto bailout;
	}

	// Reads always go on a list and are never IOF_QUICK as they get replied upon reception.
	// Broadcast or multicast is also unknown at this point.
	ioreq->ios2_Req.io_Flags &= ~( SANA2IOF_BCAST | SANA2IOF_MCAST );

	// Add I/O request to the pending reads list of the stack queue.
	ioreq->ios2_Req.io_Message.mn_Node.ln_Type	= 0;
	ioreq->ios2_Req.io_Error					= 0;

	// --

	IExec->AddTail( & unit->unit_RxList, (APTR) ioreq );

	Schedule_RxIOReq( unit );

bailout:

}

///
/// 0003 Cmd_Write

// Cmd_WRITE() simply adds the request to the transmit pending list
// only if the NIC has been configured and online.

static void Cmd_Write( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	if ( unit->unit_Detached )
	{
		SHOWMSG( "USB Device Detached" );

		ioreq->ios2_Req.io_Error	= S2ERR_OUTOFSERVICE;
		ioreq->ios2_WireError		= S2WERR_UNIT_OFFLINE;

		IExec->ReplyMsg( (APTR) ioreq );
		goto bailout;
	}

	if ( unit->unit_Online == FALSE )
	{
		SHOWMSG( "USB Device is Offline" );

		ioreq->ios2_Req.io_Error	= S2ERR_OUTOFSERVICE;
		ioreq->ios2_WireError		= S2WERR_UNIT_OFFLINE;

		IExec->ReplyMsg( (APTR) ioreq );
		goto bailout;
	}

	// --

	IExec->AddTail( & unit->unit_TxList, (APTR) ioreq );

	Schedule_TxIOReq( unit );

bailout:

}

///
/// 0008 Cmd_Flush

static void Cmd_Flush( struct devUnit *unit, struct IOSana2Req *ioreq )
{
struct BufferManagement *bm;

	/*	This command purges the read and write request queues for the
	 *	serial device. Flush will not affect active requests.
	 */

	bm = (APTR) ioreq->ios2_BufferManagement;

	if ( bm )
    {
		AbortAll( unit, bm, S2WERR_GENERIC_ERROR );

		ioreq->ios2_Req.io_Error = S2ERR_NO_ERROR;
	}
	else
	{
		ioreq->ios2_Req.io_Error = S2ERR_BAD_ARGUMENT;
	}

	IExec->ReplyMsg( (APTR) ioreq );

}

///

/// 0009 s2_DeviceQuery

static void s2_DeviceQuery( struct devUnit *unit UNUSED, struct IOSana2Req *ioreq )
{
struct Sana2DeviceQuery *query;
struct Sana2DeviceQuery DevQueryInfo =
{
/* SizeAvailable	*/ 0,
/* SizeSupplied		*/ 0,
/* DevQueryFormat	*/ 0,
/* DeviceLevel		*/ 0,
/* AddrFieldSize	*/ 48,
/* MTU				*/ ETHERNET_MTU,
/* BPS				*/ 0,
/* HardwareType		*/ S2WireType_Ethernet,
/* RawMTU			*/ ETHERNET_MTU
};

	query = (APTR) ioreq->ios2_StatData;

	if ( query == NULL )
    {
		SHOWMSG( "NULL Pointer" );

		ioreq->ios2_Req.io_Error	= S2ERR_BAD_ARGUMENT;
		ioreq->ios2_WireError		= S2WERR_NULL_POINTER;
	    goto bailout;
    }

	if ( query->SizeAvailable < offsetof( struct Sana2DeviceQuery, AddrFieldSize ))
    {
		SHOWMSG( "Query Buffer is too short" );

		ioreq->ios2_Req.io_Error	= S2ERR_BAD_ARGUMENT;
		ioreq->ios2_WireError   	= S2WERR_BAD_STATDATA;
	    goto bailout;
    }

    query->SizeSupplied = query->SizeAvailable;

    if ( query->SizeSupplied > sizeof( DevQueryInfo ))
    {
		 query->SizeSupplied = sizeof( DevQueryInfo );
    }

// -- 	  if ( unit->unit_CurrentSpeed & ( 4<<1 ) )
    {
		// 100MBits
	    DevQueryInfo.BPS = 100000000;
    }
// -- 	  else
// -- 	  {
// -- 		  // 10MBits
// -- 		  DevQueryInfo.BPS = 10000000;
// -- 	  }

    memcpy( & query->DevQueryFormat, & DevQueryInfo.DevQueryFormat,
		query->SizeSupplied - offsetof( struct Sana2DeviceQuery, DevQueryFormat ));

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 000A s2_GetStationAddress

/*
**
** s2_GetStationAddress()
**
** This routine returns the MAC address to the IO request
**
*/

static void s2_GetStationAddress( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	memcpy( & ioreq->ios2_SrcAddr[0], & unit->unit_CurrentAddress[0], 6 );
	memcpy( & ioreq->ios2_DstAddr[0], & unit->unit_DefaultAddress[0], 6 );

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 000B s2_ConfigInterface

static void s2_ConfigInterface( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	if ( IS_MULTICAST( ioreq->ios2_SrcAddr ))
    {
		SHOWMSG( "requested local address is a multicast/broadcast address" );

		ioreq->ios2_Req.io_Error	= S2ERR_BAD_ADDRESS;
		ioreq->ios2_WireError		= 0;
 	    goto bailout;
    }

	if ( IS_BROADCAST( ioreq->ios2_SrcAddr ))
    {
		SHOWMSG( "requested local address is a multicast/broadcast address" );

		ioreq->ios2_Req.io_Error	= S2ERR_BAD_ADDRESS;
		ioreq->ios2_WireError		= 0;
 	    goto bailout;
    }

	if ( unit->unit_Configured == TRUE )
    {
		SHOWMSG( "hardware has allready been configured" );

		ioreq->ios2_Req.io_Error	= IOERR_SELFTEST;
		ioreq->ios2_WireError		= 0;
	    goto bailout;
    }

	memcpy( & unit->unit_CurrentAddress[0] , & ioreq->ios2_SrcAddr[0] , 6 );

	SHOWMSG( "Setting new MAC Address - %02lx:%02lx:%02lx:%02lx:%02lx:%02lx",
		unit->unit_CurrentAddress[0],
		unit->unit_CurrentAddress[1],
		unit->unit_CurrentAddress[2],
		unit->unit_CurrentAddress[3],
		unit->unit_CurrentAddress[4],
		unit->unit_CurrentAddress[5]
	);

	// --

	if ( ADM8511_Set_Registers( unit, & unit->unit_CurrentAddress[0], ADM8511_REG_EID0, 6 ) == FALSE )
	{
		SHOWMSG( "Failed setting new MAC Address" );

		ioreq->ios2_Req.io_Error	= S2ERR_BAD_STATE;
		ioreq->ios2_WireError		= S2WERR_IS_CONFIGURED;
 	    goto bailout;
    }

	// --

	unit->unit_Configured = TRUE;
	unit->unit_Online = TRUE;

	ITimer->GetSysTime( & unit->Status_LastStart );

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 000E s2_AddMultiCastAddress

static void s2_AddMultiCastAddress( struct devUnit *unit, struct IOSana2Req *ioreq )
{
struct MulticastAddress *multicast;

	if ( unit->unit_Online == FALSE )
    {
		SHOWMSG( "this unit is currently offline" );

        ioreq->ios2_Req.io_Error= S2ERR_OUTOFSERVICE;
        ioreq->ios2_WireError   = S2WERR_UNIT_OFFLINE;

        goto bailout;
    }

	// Only done for reasonable multicast addresses.
    if ( !IS_MULTICAST( ioreq->ios2_SrcAddr ) || IS_BROADCAST( ioreq->ios2_SrcAddr ))
    {
		SHOWMSG( "this is not a multicast address" );

        ioreq->ios2_Req.io_Error= S2ERR_BAD_ADDRESS;
        ioreq->ios2_WireError   = S2WERR_BAD_MULTICAST;

        goto bailout;
    }

	SHOWMSG( "Add MultiCase MACAddress - %02lx:%02lx:%02lx:%02lx:%02lx:%02lx",
		ioreq->ios2_SrcAddr[0],
		ioreq->ios2_SrcAddr[1],
		ioreq->ios2_SrcAddr[2],
		ioreq->ios2_SrcAddr[3],
		ioreq->ios2_SrcAddr[4],
		ioreq->ios2_SrcAddr[5]
	);

	#if 1
	// Allocate resources if somebody isn't already using this address.
    multicast = FindMulticastAddress( unit, ioreq->ios2_SrcAddr );

	if ( multicast == NULL )
    {
		multicast = IExec->AllocVecTags( sizeof( struct MulticastAddress ),
			AVT_Clear, 0,
			TAG_END
		);

        if ( multicast == NULL )
        {
			SHOWMSG( "not enough memory for new address" );

	        ioreq->ios2_Req.io_Error= S2ERR_NO_RESOURCES;
	        ioreq->ios2_WireError   = 0;

	        goto bailout;
        }

        memcpy( &multicast->ma_Address, &ioreq->ios2_SrcAddr, 6 );

    	// Calculate the multicast hash bit from the address to be checked
		multicast->ma_HashBit = CalculateCRC( multicast->ma_Address );

		if ( unit->unit_Promisc == FALSE )
		{
			// We may have to enable the hash bit in the hardware if it isn't set up already.
			if ( FindMulticastAddressViaBit( unit, multicast->ma_HashBit ) == NULL )
			{
				CalcMulticastBits( unit );

				ADM8511_Set_Registers( unit, unit->unit_MulticastBits, ADM8511_REG_MC0, 8 );
			}
		}

		IExec->AddTail( & unit->unit_MultiCastList, (APTR) multicast );
    }

	// Another user of this address.
    multicast->ma_UseCount++;
	#endif

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 000F s2_DelMultiCastAddress

static void s2_DelMultiCastAddress( struct devUnit *unit, struct IOSana2Req *ioreq )
{
struct MulticastAddress *multicast;

	if ( unit->unit_Online == FALSE )
    {
		SHOWMSG( "unit is currently offline" );

        ioreq->ios2_Req.io_Error= S2ERR_OUTOFSERVICE;
        ioreq->ios2_WireError   = S2WERR_UNIT_OFFLINE;

        goto bailout;
    }

	// Only done for  reasonable multicast addresses.
    if( !IS_MULTICAST( ioreq->ios2_SrcAddr ))
    {
		SHOWMSG( "this is not a multicast address" );

        ioreq->ios2_Req.io_Error= S2ERR_BAD_ADDRESS;
        ioreq->ios2_WireError   = S2WERR_BAD_MULTICAST;

        goto bailout;
    }

	SHOWMSG( "Add MultiCase MACAddress - %02lx:%02lx:%02lx:%02lx:%02lx:%02lx",
		ioreq->ios2_SrcAddr[0],
		ioreq->ios2_SrcAddr[1],
		ioreq->ios2_SrcAddr[2],
		ioreq->ios2_SrcAddr[3],
		ioreq->ios2_SrcAddr[4],
		ioreq->ios2_SrcAddr[5]
	);

	// Can only remove multicast addresses that were previously added.
    multicast = FindMulticastAddress( unit, ioreq->ios2_SrcAddr );

    if ( multicast == NULL )
    {
		SHOWMSG( "couldn't find multicast address" );

        ioreq->ios2_Req.io_Error= S2ERR_BAD_ADDRESS;
        ioreq->ios2_WireError   = S2WERR_BAD_MULTICAST;

        goto bailout;
    }

	// If we are the last user of this address, discard its information.
    multicast->ma_UseCount--;

    if ( multicast->ma_UseCount == 0 )
    {
		SHOWMSG( "last multicast address user gone; removing it" );

		IExec->Remove( (APTR) multicast );

		if ( unit->unit_Promisc == FALSE )
		{
			// Is someone else using the same hash bit in the HW?
			if( !FindMulticastAddressViaBit( unit, multicast->ma_HashBit ))
			{
				SHOWMSG( "disabling hardware filtering for this address" );

				CalcMulticastBits( unit );

				ADM8511_Set_Registers( unit, unit->unit_MulticastBits, ADM8511_REG_MC0, 8 );
			}
		}

		IExec->FreeVec( multicast );
    }

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 0010 s2_MultiCast

// This is only done for reasonable multicast addresses
static void s2_MultiCast( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	SHOWMSG( "MultiCase DstAddress - %02lx:%02lx:%02lx:%02lx:%02lx:%02lx",
		ioreq->ios2_DstAddr[0],
		ioreq->ios2_DstAddr[1],
		ioreq->ios2_DstAddr[2],
		ioreq->ios2_DstAddr[3],
		ioreq->ios2_DstAddr[4],
		ioreq->ios2_DstAddr[5]
	);

    if ( !IS_MULTICAST( &ioreq->ios2_DstAddr[0] ) || IS_BROADCAST( &ioreq->ios2_DstAddr[0] ))
    {
		SHOWMSG( "this is not a multicast address" );

		// Multicast writes aren't accepted for non-multicast addresses
	    ioreq->ios2_Req.io_Error= S2ERR_BAD_ADDRESS;
	    ioreq->ios2_WireError   = S2WERR_BAD_MULTICAST;

		IExec->ReplyMsg( (APTR) ioreq );

	    goto bailout;
    }

	// Just like any other write.
    Cmd_Write( unit, ioreq );

bailout:

}

///
/// 0011 s2_BroadCast

// Broadcast is simply a write to the broadcast address
static void s2_BroadCast( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	memset( & ioreq->ios2_DstAddr[0], 0xff, 6 );

    Cmd_Write( unit, ioreq );

}

///
/// 0015 s2_GetSpecialStats

static void s2_GetSpecialStats(	struct devUnit *unit, struct IOSana2Req *ioreq )
{
struct Sana2SpecialStatHeader *ssh;
struct Sana2SpecialStatRecord *ssr;
U32 records;
U32 i;

	ssh = (APTR) ioreq->ios2_StatData;

    if ( ssh == NULL )
    {
		SHOWMSG( "no stat data pointer provided" );

	    ioreq->ios2_Req.io_Error= S2ERR_BAD_ARGUMENT;
	    ioreq->ios2_WireError   = S2WERR_NULL_POINTER;
	    goto bailout;
    }

	ssr = (APTR) & ssh[1];

    records = 0;

    for( i = 0 ; i < ssh->RecordCountMax ; i++ )
    {
	    switch( i )
	    {
		    case 0:
		    {
				ssr[records].Type   = 			S2SS_ETHERNET_BADMULTICAST;
				ssr[records].String = (STR)	"Bad multicast filtering";
				ssr[records].Count  = 			unit->unit_MultiCastRejects;

			    records++;

			    break;
            }

		    case 1:
            {
				ssr[records].Type   = 			S2SS_ETHERNET_FIFO_UNDERRUNS;
				ssr[records].String = (STR)	"FIFO underruns";
				ssr[records].Count  = 			unit->Status_Underruns;

			    records++;

			    break;
            }

		    default:
            {
			    break;
            }
	    }
    }

    ssh->RecordCountSupplied = records;

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 0016 s2_GetGlobalStats

static void s2_GetGlobalStats( struct devUnit *unit, struct IOSana2Req *ioreq )
{
struct Sana2DeviceStats *stats;

    if ( ioreq->ios2_StatData == NULL )
    {
		SHOWMSG( "NULL Pointer" );

	    ioreq->ios2_Req.io_Error= S2ERR_BAD_ARGUMENT;
	    ioreq->ios2_WireError   = S2WERR_NULL_POINTER;
	    goto bailout;
    }

    stats = ioreq->ios2_StatData;

    stats->PacketsReceived	    = unit->Status_PacketsReceived;
    stats->PacketsSent		    = unit->Status_PacketsSent;
    stats->BadData			    = unit->Status_BadData;
    stats->Overruns			    = unit->Status_Overruns;
	stats->Unused				= 0;
    stats->UnknownTypesReceived = unit->Status_UnknownTypesReceived;
    stats->Reconfigurations	    = unit->Status_Reconfigurations;
	stats->LastStart			= unit->Status_LastStart;

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 0017 s2_OnEvent

static void s2_OnEvent( struct devUnit *unit, struct IOSana2Req *ioreq )
{
U32 events;

    events = ioreq->ios2_WireError;

	SHOWMSG( "events to track = 0x%08lx", events );

    if ( events == 0 )
    {
		SHOWMSG( "no events given" );

	    ioreq->ios2_Req.io_Error= S2ERR_BAD_ARGUMENT;
	    ioreq->ios2_WireError   = S2WERR_BAD_EVENT;

		IExec->ReplyMsg( (APTR) ioreq );
	    goto bailout;
    }

	if (( events & S2EVENT_ONLINE ) && ( unit->unit_Online == TRUE ))
    {
		ioreq->ios2_WireError = S2EVENT_ONLINE;

		IExec->ReplyMsg( (APTR) ioreq );
	    goto bailout;
	}

	if (( events & S2EVENT_OFFLINE ) && ( unit->unit_Online == FALSE ))
    {
		ioreq->ios2_WireError = S2EVENT_OFFLINE;

		IExec->ReplyMsg( (APTR) ioreq );
	    goto bailout;
	}

	IExec->AddTail( & unit->unit_EventsList, (APTR) ioreq );

bailout:

}

///
/// 0018 s2_ReadOrphan

// S2_READORPHAN is really equal to a standard CMD_READ except
// for which packet list is uses.

static void s2_ReadOrphan( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	Cmd_Read( unit, ioreq );

}

///
/// 0019 s2_Online

static void s2_Online( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	if ( unit->unit_Detached )
	{
		SHOWMSG( "Device has be detached" );

		ioreq->ios2_Req.io_Error	= S2ERR_BAD_STATE;
		ioreq->ios2_WireError		= S2WERR_UNIT_DISCONNECTED;
        goto bailout;
	}

	if ( unit->unit_Configured == FALSE )
    {
		SHOWMSG( "hardware is not yet configured" );

		ioreq->ios2_Req.io_Error	= IOERR_SELFTEST;
		ioreq->ios2_WireError		= 0;
 	    goto bailout;
    }

	if ( unit->unit_Online == TRUE )
    {
		SHOWMSG( "we are already online" );

 	    ioreq->ios2_Req.io_Error= S2ERR_BAD_STATE;
 	    ioreq->ios2_WireError   = S2WERR_UNIT_ONLINE;
 	    goto bailout;
    }

	unit->unit_Online = TRUE;

	ITimer->GetSysTime( & unit->Status_LastStart );

    CheckEvent( unit, S2EVENT_ONLINE );

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// 001A s2_Offline

static void s2_Offline( struct devUnit *unit, struct IOSana2Req *ioreq )
{
	if ( unit->unit_Online == FALSE )
    {
		SHOWMSG( "we are already offline" );

		ioreq->ios2_Req.io_Error	= S2ERR_OUTOFSERVICE;
		ioreq->ios2_WireError		= S2WERR_UNIT_OFFLINE;
	    goto bailout;
    }

	unit->unit_Online = FALSE;

    AbortAll( unit, NULL, S2WERR_UNIT_OFFLINE );

    CheckEvent( unit, S2EVENT_OFFLINE );

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///

/// 4000 NSCmd_DeviceQuery

static void NSCmd_DeviceQuery( struct devUnit *unit UNUSED, struct IOStdReq *ioreq )
{
struct NSDeviceQueryResult *nsd;

	static uint16 _DevNSDCmds[] =
	{
		CMD_INVALID,
		CMD_READ,
		CMD_WRITE,
		CMD_FLUSH,
		NSCMD_DEVICEQUERY,
		S2_SANA2HOOK,
		S2_DEVICEQUERY,
		S2_GETSTATIONADDRESS,
		S2_CONFIGINTERFACE,
	    S2_ADDMULTICASTADDRESS,
	    S2_DELMULTICASTADDRESS,
		S2_MULTICAST,
		S2_BROADCAST,
//		  S2_TRACKTYPE,
//		  S2_UNTRACKTYPE,
//		  S2_GETTYPESTATS,
		S2_GETSPECIALSTATS,
		S2_GETGLOBALSTATS,
		S2_ONEVENT,
		S2_READORPHAN,
	    S2_ONLINE,
	    S2_OFFLINE,
		S2_GETEXTENDEDGLOBALSTATS,
		S2_SAMPLE_THROUGHPUT,
	    0
	};

	nsd = ioreq->io_Data;

	if (( nsd != NULL ) && ( ioreq->io_Length >= sizeof( struct NSDeviceQueryResult )))
    {
    	// (the size check is only valid because the current NSDDeviceQueryResult
    	//  structure is defined as the minimum version of that struct!)
		nsd->DevQueryFormat		= 0;
		nsd->SizeAvailable		= sizeof( struct NSDeviceQueryResult );
		nsd->DeviceType			= NSDEVTYPE_SANA2;
		nsd->DeviceSubType		= 0;
		nsd->SupportedCommands	= (APTR)_DevNSDCmds;

        ioreq->io_Actual		= sizeof( struct NSDeviceQueryResult );
        ioreq->io_Error			= 0;
    }
    else
    {
		SHOWMSG( "Supplied Query buffer is to small" );

        ioreq->io_Error			= IOERR_BADLENGTH;
    }

	// NSD query performed
	IExec->ReplyMsg( (APTR) ioreq );

}

///

/// C004 s2_GetExtendedGlobalStats

static void s2_GetExtendedGlobalStats( struct devUnit *unit, struct IOSana2Req *ioreq )
{
struct Sana2ExtDeviceStats stats;
U32 len;

	len = ((struct Sana2ExtDeviceStats *)ioreq->ios2_StatData)->s2xds_Length;

	if ( len < 8 )
    {
		SHOWMSG( "Structure is too short (%ld < 8)", len );

	    ioreq->ios2_Req.io_Error= IOERR_BADLENGTH;
	    ioreq->ios2_WireError   = 0;
	    goto bailout;
    }

	memset( &stats, 0, sizeof( struct Sana2ExtDeviceStats ));

	stats.s2xds_Length				    = len;
	stats.s2xds_Actual				    = sizeof( struct Sana2ExtDeviceStats );
    stats.s2xds_PacketsReceived		    = unit->Status_PacketsReceived;
    stats.s2xds_PacketsSent			    = unit->Status_PacketsSent;
    stats.s2xds_BadData				    = unit->Status_BadData;
    stats.s2xds_Overruns			    = unit->Status_Overruns;
    stats.s2xds_UnknownTypesReceived    = unit->Status_UnknownTypesReceived;
    stats.s2xds_Reconfigurations	    = unit->Status_Reconfigurations;
    stats.s2xds_LastStart			    = unit->Status_LastStart;

	if ( stats.s2xds_Actual > stats.s2xds_Length )
    {
		 stats.s2xds_Actual = stats.s2xds_Length ;
    }

	memcpy( ioreq->ios2_StatData, & stats, stats.s2xds_Actual );

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///
/// C007 s2_Sample_ThroughPut

static void s2_Sample_ThroughPut( struct devUnit *unit, struct IOSana2Req *ioreq )
{
struct Sana2ThroughputStats *stats;

	stats = ioreq->ios2_StatData;

	if ( stats->s2ts_Length < 8 )
    {
		SHOWMSG( "structure is too short (%ld < 8)", stats->s2ts_Length );

	    ioreq->ios2_Req.io_Error= IOERR_BADLENGTH;
	    ioreq->ios2_WireError   = 0;

		IExec->ReplyMsg( (APTR) ioreq );
	    goto bailout;
    }

	stats->s2ts_Actual = MIN( stats->s2ts_Length , sizeof( struct Sana2ThroughputStats ));

	memset( & stats->s2ts_NotifyTask, 0, stats->s2ts_Actual - offsetof( struct Sana2ThroughputStats, s2ts_NotifyTask ));

	IExec->AddTail( & unit->unit_SampleList, (APTR) ioreq );

bailout:

}

///
/// C008 s2_Sana2Hook

static void s2_Sana2Hook( struct devUnit *unit UNUSED, struct IOSana2Req *ioreq )
{
struct BufferManagement *bm;
struct Sana2Hook *hook;

	if ( ioreq->ios2_DataLength < 20 )
	{
		SHOWMSG( "Structure is too short (%ld < 20)", ioreq->ios2_DataLength );

		ioreq->ios2_Req.io_Error	= IOERR_BADLENGTH;
		ioreq->ios2_WireError		= 0;
        goto bailout;
	}

	bm		= (APTR) ioreq->ios2_BufferManagement;
	hook	= (APTR) ioreq->ios2_Data;

	bm->bm_CopyHook = & hook->s2h_Hook;

	if (( IUtility->TagInArray( S2_CopyToBuff,	 hook->s2h_Methods ) == FALSE )
	||	( IUtility->TagInArray( S2_CopyFromBuff, hook->s2h_Methods ) == FALSE ))
	{
		SHOWMSG( "mandatory tags are not provided" );

		ioreq->ios2_Req.io_Error	= S2ERR_BAD_ARGUMENT;
		ioreq->ios2_WireError		= S2WERR_FUNCTIONS_MISSING;

		bm->bm_CopyHook = NULL;
        goto bailout;
	}

	// the opener has the required hooks for native calls
	bm->bm_CopyToBuff			= NULL;
	bm->bm_CopyFromBuff			= NULL;
	bm->bm_CopyToBuffTag		= S2_CopyToBuff;
	bm->bm_CopyFromBuffTag		= S2_CopyFromBuff;
	bm->bm_DMACopyToBuffTag		= 0;
	bm->bm_DMACopyFromBuffTag	= 0;

	if ( IUtility->TagInArray( S2_DMACopyToBuff32, hook->s2h_Methods ))
    {
		bm->bm_DMACopyToBuffTag = S2_DMACopyToBuff32;
    }

	if ( IUtility->TagInArray( S2_DMACopyFromBuff32, hook->s2h_Methods ))
    {
		bm->bm_DMACopyFromBuffTag = S2_DMACopyFromBuff32;
    }

bailout:

	IExec->ReplyMsg( (APTR) ioreq );

}

///

#endif
