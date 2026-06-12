
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
** Copyright (c) 2001 The NetBSD Foundation, Inc.
** All rights reserved.
**
** This code is derived from software contributed to The NetBSD Foundation
** by Lennart Augustsson (lennart@augustsson.net).
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
** ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_HCD_EHCI_H
#define INC_PRIVATE_HCD_EHCI_H

/***************************************************************************/
// Defines based on old OpenBSD ehcireg.h

#define EHCI_LINK_TERMINATE			0x00000001UL
#define EHCI_LINK_QH				0x02U
//#define EHCI_QH_SPEED_FULL			0x00U
//#define EHCI_QH_SPEED_LOW			0x01U
//#define EHCI_QH_SPEED_HIGH			0x02U
//#define EHCI_QH_DTC					0x00004000UL
//#define EHCI_QH_HRECL				0x00008000UL
//#define EHCI_QH_CTL					0x08000000UL
//#define EHCI_QH_SET_ADDR(x)			(x)
//#define EHCI_QH_SET_SMASK(x)		((x) <<  0)
//#define EHCI_QH_SET_CMASK(x)		((x) <<  8)
//#define EHCI_QH_SET_ENDPT(x)		((x) <<  8)
//#define EHCI_QH_SET_EPS(x)			((x) << 12)
//#define EHCI_QH_SET_MPL(x)			((x) << 16)
//#define EHCI_QH_SET_HUBA(x)			((x) << 16)
//#define EHCI_QH_SET_PORT(x)			((x) << 23)
//#define EHCI_QH_SET_NRL(x)			((x) << 28)
//#define EHCI_QH_SET_MULT(x)			((x) << 30)

//#define USB_PORT_ROOT_RESET_DELAY	100				// ms
//#define EHCI_PORT_RESET_COMPLETE	10				// ms

#define EHCI_NULL					LE_SWAP32( EHCI_LINK_TERMINATE )

/* Poll rates (1ms, 2, 4, 8 .. 128) */
#define EHCI_IPOLLRATES				8	

#define EHCI_IQHIDX(lev, pos)		(((( pos ) & (( 1 << ( lev-1 )) - 1 )) | ( 1 << ( lev-1 ))) - 1 )

/***************************************************************************/
// EHCI Capability Registers

#define EHCI_CAPLENGTH				0x00U			// RO : Capability Register Length
#define EHCI_HCIVERSION				0x02U			// RO : Interface Version Number (BCD)
#define EHCI_HCSPARAMS				0x04U			// RO : Structural Parameters 
#define EHCI_HCCPARAMS				0x08U			// RO : Capability Parameters
#define EHCI_HCSP_PORTROUTE			0x0CU			// RO : Companion Port Route Description

// -- HCSPARAMS
#define	EHCI_HCS_N_PORTS(x)			((x) & 0x0000000FUL )
#define	EHCI_HCS_P_INDICATOR(x) 	((x) & 0x00010000UL )

/***************************************************************************/
// EHCI Operation Registers

#define EHCI_USBCMD					0x00U			// RW : USB Command
#define EHCI_USBSTS					0x04U			// RW : USB Status
#define EHCI_USBINTR				0x08U			// RW : USB Interrupt Enable
#define EHCI_FRINDEX				0x0CU			// RW : USB Frame Index
#define EHCI_PERIOD					0x14U			// RW : Frame List Base Address
#define EHCI_ASYNC					0x18U			// RW : Next Asynchronous List Address
#define EHCI_CONFIGFLAG				0x40U			// RW : Configured Flag Register
// Ports from 0x44 .. but 0x44 is port 1, so we do 0x40 and use Zero based index
#define EHCI_PORTSC(x)				(0x40+(4*x))	// RW : Port Status/Control Register 

// -- USBCMD
#define	EHCI_CMD_RS					0x00000001UL	// RW : Run/Stop
#define	EHCI_CMD_HCRESET			0x00000002UL	// RW : Reset
#define EHCI_CMD_FLS_MASK			0x0000000CUL	// RW/RO : Frame list size
#define EHCI_CMD_PSE				0x00000010UL	// RW : Periodic sched enable
#define EHCI_CMD_ASE				0x00000020UL	// RW : Async sched enable
#define EHCI_CMD_IAAD				0x00000040UL	// RW : intr on async adv door bell
#define EHCI_CMD_ITC_1				0x00010000UL	// 1 microframe
#define EHCI_CMD_ITC_2				0x00020000UL	// 2 microframe
#define EHCI_CMD_ITC_4				0x00040000UL	// 4 microframe
#define EHCI_CMD_ITC_8				0x00080000UL	// 8 microframe
#define EHCI_CMD_ITC_16				0x00100000UL	// 16 microframe
#define EHCI_CMD_ITC_32				0x00200000UL	// 32 microframe
#define EHCI_CMD_ITC_64				0x00400000UL	// 64 microframe
#define EHCI_CMD_FLS_256			0x00000008UL
#define EHCI_CMD_FLS_512			0x00000004UL
#define EHCI_CMD_FLS_1024			0x00000000UL

// -- USBSTS
#define EHCI_STS_INT				0x00000001UL	// RWC : interrupt
#define EHCI_STS_ERRINT				0x00000002UL	// RWC : error interrupt
#define EHCI_STS_PCD				0x00000004UL	// RWC : port change detect
#define EHCI_STS_FLR				0x00000008UL	// RWC : frame list rollover
#define EHCI_STS_HSE				0x00000010UL	// RWC : host system error
#define EHCI_STS_IAA				0x00000020UL	// RWC : interrupt on async adv
#define EHCI_STS_HCH				0x00001000UL	// RO : Host Controller Halted
#define EHCI_STS_REC				0x00002000UL	// RO : reclamation
#define EHCI_STS_PSS				0x00004000UL	// RO : periodic sched status
#define EHCI_STS_ASS				0x00008000UL	// RO : async sched status
#define EHCI_STS_INTRS				( EHCI_STS_INT | EHCI_STS_ERRINT | EHCI_STS_PCD | EHCI_STS_FLR | EHCI_STS_HSE | EHCI_STS_IAA | EHCI_STS_HCH )

// -- USBINTR
#define	EHCI_INTR_UIE				0x00000001		// RW : USB Interrupt Enable
#define	EHCI_INTR_UEIE				0x00000002		// RW : USB Error Interrupt Enable
#define	EHCI_INTR_PCDIE				0x00000004		// RW : Port Change Detect Enable
#define	EHCI_INTR_FLRE				0x00000008		// RW : Frame List Rollover Enable
#define	EHCI_INTR_HSEE				0x00000010		// RW : Host System Error Enable
#define	EHCI_INTR_IAAE				0x00000020		// RW : Interrupt on Async Advance Enable
#define EHCI_INTR_NORMAL			( EHCI_INTR_IAAE | EHCI_INTR_HSEE | EHCI_INTR_PCDIE | EHCI_INTR_UEIE | EHCI_INTR_UIE )
#define EHCI_INTR_EVENTS			( EHCI_INTR_UIE  | EHCI_INTR_PCDIE | EHCI_INTR_IAAE )
#define EHCI_INTR_ERRORS			( EHCI_INTR_UEIE | EHCI_INTR_HSEE )

// -- CONFIGFLAG
#define	EHCI_CONF_CF				0x00000001UL	// RW : Configure flag

// -- PORTSC
#define EHCI_PS_CSC					0x00000002UL	// RWC : Connect status change
#define EHCI_PS_PE					0x00000004UL	// RW  : Port enable
#define EHCI_PS_PEC					0x00000008UL	// RWC : Port enable change
#define EHCI_PS_OCC					0x00000020UL	// RWC : Over current change
#define EHCI_PS_PR					0x00000100UL	// RW  : Port reset
#define EHCI_PS_LS					0x00000c00UL	// RO  : Line Status
#define EHCI_PS_PP					0x00001000UL	// RW,RO : Port power
#define EHCI_PS_PO					0x00002000UL	// RW  : Port owner
#define EHCI_PS_IS_LOWSPEED(x)		(((x) & EHCI_PS_LS ) == 0x00000400UL )
#define EHCI_PS_CLEAR				( EHCI_PS_OCC | EHCI_PS_PEC | EHCI_PS_CSC )

//#define EHCI_PORTSC(n)		(0x40+4*(n)) /* RO, RW, RWC Port Status reg */
//#define  EHCI_PS_WKOC_E		0x00400000 /* RW wake on over current ena */
//#define  EHCI_PS_WKDSCNNT_E	0x00200000 /* RW wake on disconnect ena */
//#define  EHCI_PS_WKCNNT_E	0x00100000 /* RW wake on connect ena */
//#define  EHCI_PS_PTC		0x000f0000 /* RW port test control */
//#define  EHCI_PS_PIC		0x0000c000 /* RW port indicator control */
//#define  EHCI_PS_PP		0x00001000 /* RW,RO port power */
//#define  EHCI_PS_LS		0x00000c00 /* RO line status */
//#define  EHCI_PS_IS_LOWSPEED(x)	(((x) & EHCI_PS_LS) == 0x00000400)
//#define  EHCI_PS_PR		0x00000100 /* RW port reset */
//#define  EHCI_PS_SUSP		0x00000080 /* RW suspend */
//#define  EHCI_PS_FPR		0x00000040 /* RW force port resume */
//#define  EHCI_PS_OCC		0x00000020 /* RWC over current change */
//#define  EHCI_PS_OCA		0x00000010 /* RO over current active */
//#define  EHCI_PS_PEC		0x00000008 /* RWC port enable change */
//#define  EHCI_PS_CSC		0x00000002 /* RWC connect status change */
//#define  EHCI_PS_CS		0x00000001 /* RO connect status */

/***************************************************************************/
// Queue Header (QH) Structure

struct EHCI_QH
{
/* --    Standard - Hardware - Don't change */
/* 00 */ U32				qh_link;				// Physical addresse
/* 04 */ U32				qh_ep1;					// Flags - Endpoint Characteristics
/* 08 */ U32				qh_ep2;					// Flags - Endpoint Capabilities
/* 0C */ U32				qh_td_cur;				// Physical addresse
/* --    Overlay  - Hardware - Don't change */
/* 10 */ U32				qh_td_next;				// Physical addresse
/* 14 */ U32				qh_td_alt;				// Physical addresse
/* 18 */ U32				qh_status;				// Flags - Status / Control info
/* 1C */ U32				qh_buffer[5];			// Physical addresses
/* 30    Private - Software */
	U32						pqh_self_phy;			// Physical addresse
	struct EHCI_QH *		pqh_virt_next;			// Virtual address
	struct EHCI_TD *		pqh_first_td;			// Virtual address
};

#define EHCI_QH_ALIGN			32						// Struct min alignment
//#define EHCI_QH_SIZE			(( sizeof( struct EHCI_QH ) + EHCI_QH_ALIGN - 1 ) / EHCI_QH_ALIGN** EHCI_QH_ALIGN )
//#define EHCI_QH_CHUNK			( EHCI_PAGE_SIZE / EHCI_QH_SIZE )
#define EHCI_QH_GET_ADDR(x)		(((x) >>  0) & 0x7f)	// endpoint addr
#define EHCI_QH_SET_ADDR(x)		(x)
//#define EHCI_QH_ADDRMASK		0x0000007fU
#define EHCI_QH_GET_INACT(x)	(((x) >>  7) & 0x01)	// inactivate on next
//#define EHCI_QH_INACT			0x00000080U
#define EHCI_QH_GET_ENDPT(x)	(((x) >>  8) & 0x0f)	// endpoint no
#define EHCI_QH_SET_ENDPT(x)	( (x) <<  8)
#define EHCI_QH_GET_EPS(x)		(((x) >> 12) & 0x03)	// endpoint speed
#define EHCI_QH_SET_EPS(x)		( (x) << 12)
#define EHCI_QH_SPEED_FULL		0x00U
#define EHCI_QH_SPEED_LOW		0x01U
#define EHCI_QH_SPEED_HIGH		0x02U
#define EHCI_QH_GET_DTC(x)		(((x) >> 14) & 0x01)	// data toggle control
#define EHCI_QH_DTC				0x00004000U
#define EHCI_QH_GET_HRECL(x)	(((x) >> 15) & 0x01)	// head of reclamation
#define EHCI_QH_HRECL			0x00008000U
#define EHCI_QH_GET_MPL(x)		(((x) >> 16) & 0x7ff)	// max packet len
#define EHCI_QH_SET_MPL(x)		( (x) << 16)
//#define EHCI_QH_MPLMASK		0x07ff0000
#define EHCI_QH_GET_CTL(x)		(((x) >> 27) & 0x01)	// control endpoint
#define EHCI_QH_CTL				0x08000000U
#define EHCI_QH_GET_NRL(x)		(((x) >> 28) & 0x0f)	// NAK reload
#define EHCI_QH_SET_NRL(x)		( (x) << 28)

#define EHCI_QH_GET_SMASK(x)	(((x) >>  0) & 0xff)	// intr sched mask
#define EHCI_QH_SET_SMASK(x)	( (x) <<  0)
#define EHCI_QH_GET_CMASK(x)	(((x) >>  8) & 0xff)	// split completion mask
#define EHCI_QH_SET_CMASK(x)	( (x) <<  8)
#define EHCI_QH_GET_HUBA(x)		(((x) >> 16) & 0x7f)	// hub address
#define EHCI_QH_SET_HUBA(x)		( (x) << 16)
#define EHCI_QH_GET_PORT(x)		(((x) >> 23) & 0x7f)	// hub port
#define EHCI_QH_SET_PORT(x)		( (x) << 23)
#define EHCI_QH_GET_MULT(x)		(((x) >> 30) & 0x03)	// pipe multiplier
#define EHCI_QH_SET_MULT(x)		( (x) << 30)

/***************************************************************************/
// Transfer Descriptor (TD) Structure

struct EHCI_TD
{
	// Standard - Hardware - Don't change
	U32						td_td_next;				// Physical addresses
	U32						td_td_alt;				// Physical addresses
	U32						td_status;				// Flags - Status / Control info
	U32						td_buffer[5];			// Physical addresses
	// Private - Software
	U32						ptd_self_phy;			// Physical addresse
	struct EHCI_TD *		ptd_virt_next;			// Virtual address
	U32						ptd_buffer_phy;			// Physical addresse
	PTR						ptd_buffer_virt;		// Virtual start address
	U32						ptd_buffer_size;		//
};

#define EHCI_TD_ACTIVE			0x80U
#define EHCI_TD_HALTED			0x40U
#define EHCI_TD_BUFERR			0x20U
#define EHCI_TD_BABBLE			0x10U
#define EHCI_TD_XACTERR			0x08U
//#define EHCI_TD_MISSEDMICRO		0x04U
//#define EHCI_TD_SPLITXSTATE		0x02U
#define EHCI_TD_PINGSTATE		0x01U
#define EHCI_TD_STATERRS		0x7CU
#define EHCI_TD_PID_OUT			0x00U
#define EHCI_TD_PID_IN			0x01U
#define EHCI_TD_PID_SETUP		0x02U
#define EHCI_TD_IOC				0x00008000UL
//#define EHCI_TD_TOGGLE_MASK		0x80000000UL
#define EHCI_TD_ALIGN			32					// Struct min alignment
//#define EHCI_TD_SIZE			(( sizeof( struct EHCI_TD ) + EHCI_TD_ALIGN - 1 ) / EHCI_TD_ALIGN** EHCI_TD_ALIGN )
//#define EHCI_TD_CHUNK			( EHCI_PAGE_SIZE / EHCI_TD_SIZE )
#define EHCI_TD_GET_STATUS(x)	(((x) >>  0) & 0xff)
#define EHCI_TD_SET_STATUS(x)	( (x) <<  0)
#define EHCI_TD_GET_PID(x)		(((x) >>  8) & 0x3)
#define EHCI_TD_SET_PID(x)		( (x) <<  8)
#define EHCI_TD_GET_CERR(x)		(((x) >> 10) &  0x3)
#define EHCI_TD_SET_CERR(x)		( (x) << 10)
#define EHCI_TD_GET_C_PAGE(x)	(((x) >> 12) &  0x7)
//#define EHCI_TD_SET_C_PAGE(x)	( (x) << 12)
#define EHCI_TD_GET_IOC(x)		(((x) >> 15) &  0x1)
#define EHCI_TD_GET_BYTES(x)	(((x) >> 16) &  0x7fff)
#define EHCI_TD_SET_BYTES(x)	( (x) << 16)
#define EHCI_TD_GET_TOGGLE(x)	(((x) >> 31) &  0x1)
#define EHCI_TD_SET_TOGGLE(x)	( (x) << 31)

/***************************************************************************/
// todo

struct EHCI_ITD
{
	int dummy;
};

#define EHCI_ITD_ALIGN			32					// Struct min alignment
#define EHCI_ITD_SIZE			(( sizeof( struct EHCI_ITD ) + EHCI_ITD_ALIGN - 1 ) / EHCI_ITD_ALIGN** EHCI_ITD_ALIGN )
#define EHCI_ITD_CHUNK			( EHCI_PAGE_SIZE / EHCI_ITD_SIZE )

/***************************************************************************/

struct _EHCI
{
	U32								CapLength;

	struct USB3_Signal				Signal_EI;
	struct USB3_Signal				Signal_INT;
	struct USB3_Signal				Signal_IAA;

	#if 0
//	U32								SParams;
	#endif

	struct EHCI_QH *				ASync_Head;

	U8 *							PortResetChange;

	// Interrupt FrameList
	U32 *							FrameList;
	U32								FrameList_Size;
	U32								FrameList_Phyaddr;
	struct EHCI_QH **				FrameList_ISlots;
	S32								BandWidth[ 256 ];		// hmm alloc mem instead? like ISlots

	#if 0
//	U32								FrameList_LevelCounter[ 8 ];
	#endif
};

/***************************************************************************/

extern const struct HCDFunctions EHCIFunctions;

/***************************************************************************/

#endif // INC_PRIVATE_HCD_EHCI_H
