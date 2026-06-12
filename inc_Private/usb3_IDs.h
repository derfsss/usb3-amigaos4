
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_USB3_IDS_H
#define INC_PRIVATE_USB3_IDS_H

/***************************************************************************/
// ID's are just random 32bit numbers
// From : https://numbergenerator.org/random-32-digit-hex-codes-generator#!numbers=8&length=8

// --
#define ID_USB2_FREED		0xCBFEA871		// Structure have been Freed
// --
#define ID_USB2_EPR			0x55CF027E		// struct RealEndPointResource
#define ID_USB2_IOR			0x03534DC1		// struct RealRequest
#define ID_USB2_REG  		0xD2E37DDA		// struct RealRegistrer
#define ID_USB2_SD  		0x1AB72537		// struct RealSetupData
// --
#define ID_USB2_ASYNC		0xF8A32493		// struct USB2_ASync
#define ID_USB2_CFG  		0x5D8B0340		// struct USB2_ConfigNode
#define ID_USB2_DN			0x0D964139		// struct USB2_DriverNode
#define ID_USB2_EP			0xEBA71F15		// struct USB2_EndPointNode
#define ID_USB2_FDN			0x91868623		// struct USB2_FktDriverNode
#define ID_USB2_FKT			0xBB9A0BBE		// struct RealFunctionNode
#define ID_USB2_HN			0x343D8884		// struct USB2_HCDNode
#define ID_USB2_IFCG		0xC5266E8C		// struct USB2_InterfaceGroup
#define ID_USB2_IFCH		0xE1565521		// struct USB2_InterfaceHeader
#define ID_USB2_IFCN		0xAF02474F		// struct USB2_InterfaceNode
#define ID_USB2_MP			0x0E46CDE0		// struct USB2_MsgPort
#define ID_USB2_TMSG		0xC42C9576		// struct USB2_TaskMsg
#define ID_USB2_SEMA		0xC09CE5F2		// struct USB2_Semaphore
#define ID_USB2_SIG			0x756DCC2A		// struct USB2_Signal
#define ID_USB2_TN			0x3BA9D5E8		// struct USB2_TaskNode
#define ID_USB2_NN			0xAA1F5CB3		// struct USB2_NotifyNode

#ifdef DO_DEBUG
#define	ID_IN_MASTER		0x545CCF7B		// Intern struct For Master Task
#define	ID_IN_HCD			0x5C993B19		// Intern struct For HCD Task
#define	ID_IN_HUB			0x8EAED55E		// Intern struct For HUB Task
#define	ID_IN_HID			0x6C583C8F		// Intern struct For HID Task
#endif

// --


#if 0


4A640E26
32A0DBBA
F558C6DD
796F51D9
9A74EAAB
0725D314
F96328DA
1B451232
95490729
58A730A1
82F4965C
17A47958
B43097A7
EACBDCBF
3AF0A7EC


#define StructID_LOG    1661
#define StructID_MDN	2442	// Masstorage Device [Unit] Node
//#define StructID_ENV	  ( (U32)'S'<<24 | (U32)'E'<<16 | (U32)'N'<<8 | (U32)'V'<<0 )

#endif

/***************************************************************************/

#endif // INC_PRIVATE_USB3_IDS_H
