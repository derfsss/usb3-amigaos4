
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INC_PRIVATE_USB3_ALL_H
#error Include "usb3_all.h" first
#endif

#ifndef INC_PRIVATE_HUB_H
#define INC_PRIVATE_HUB_H

/***************************************************************************/

#pragma pack(1)

/***************************************************************************/

#define HUB_Ifc_Nr		0U
#define HUB_Cfg_Nr		1U
#define HUB_IntEp_Nr	1U		// Interrupt EndPoint Nr

struct myHUB_Cfg
{
	struct USB2_Config_Desc			cn;
	struct USB2_Interface_Desc		ifc;
	struct USB2_EndPoint_Desc		ep;
};

/***************************************************************************/

/* Hub specific request */
//#define REQCODE_Get_Bus_State		0x02
//#define REQCODE_Clear_TT_Buffer	0x08
//#define REQCODE_Reset_TT			0x09
//#define REQCODE_Get_TT_State		0x0a
//#define REQCODE_Stop_TT			0x0b
//#define REQCODE_Set_Depth			0x0c

//--#define USBD_RESTART_MAX 		20
#define DSCTYPE_HUB					0x29
#define DSCTYPE_SS_HUB				0x2A	/* Super Speed */

/***************************************************************************/
// usb2 stack HUB bits
// Each OHCI/UHCI/EHCI bits get converted into those Bits

// Status Bits
#define HUBF_Status_Connection			0x0001U
#define HUBF_Status_Enable				0x0002U
#define HUBF_Status_Suspend				0x0004U
#define HUBF_Status_Over_Current		0x0008U
#define HUBF_Status_Reset				0x0010U
#define HUBF_Status_Power				0x0020U
#define HUBF_Status_Low_Speed			0x0040U
#define HUBF_Status_High_Speed			0x0080U
#define HUBF_Status_Test				0x0100U
#define HUBF_Status_Indicator			0x0200U

// Change Bits
#define HUBF_Chg_Connection				0x0400U
#define HUBF_Chg_Enable					0x0800U
#define HUBF_Chg_Suspend				0x1000U
#define HUBF_Chg_Over_Current			0x2000U
#define HUBF_Chg_Reset					0x4000U

/***************************************************************************/
// HUB Class Request Codes

enum
{
	GET_STATUS = 0,
	CLEAR_FEATURE,
	GET_STATE,
	SET_FEATURE,
	RESERVED1,
	RESERVED2,
	GET_DESCRIPTOR,
	SET_DESCRIPTOR,
	CLEAR_TASK_Type_BUFFER,
	RESET_TT,
	GET_TASK_Type_STATE,
	STOP_TT
};

// HUB Class Feature Selectors

enum
{
	// Hub Class Feature Selectors
	HUB_CLR_LOCAL_POWER		= 0,	// Clear hub local power lost
	HUB_CLR_OVER_CURRENT	= 1,	// Clear hub over-current

	// Port Feature Selectors (SetFeature / ClearFeature)
	PORT_CONNECTION			= 0,
	PORT_ENABLE				= 1,
	PORT_SUSPEND			= 2,
	PORT_OVER_CURRENT		= 3,
	PORT_RESET				= 4,
	PORT_L1					= 5,	// For USB 3.0 LPM (optional)

	PORT_POWER				= 8,
	PORT_LOW_SPEED			= 9,
	PORT_HIGH_SPEED			= 10,	// Not standard; informational

	// Port Status Change Feature Selectors (used in ClearFeature)
	PORT_CLR_CONNECTION		= 16,
	PORT_CLR_ENABLE			= 17,
	PORT_CLR_SUSPEND		= 18,
	PORT_CLR_OVER_CURRENT	= 19,
	PORT_CLR_RESET			= 20,

	// Optional or EHCI-specific
	PORT_TEST				= 21,	// For test mode (per EHCI spec)
	PORT_INDICATOR			= 22	// Port status LEDs (optional)
};

struct USB2_HUBStatus
{
	U16		wHubStatus;
	U16		wHubChange;
};

#define HUBSTATF_Local_Power			0x01U
#define HUBSTATF_Over_Current			0x02U

#define HUBCHGF_Local_Power_Chg		0x01U
#define HUBCHGF_Over_Current_Chg		0x02U

struct USB2_PortStatus
{
	U16		wPortStatus;
	U16		wPortChange;
};

// Status Bits
#define PORTSTATF_Connection			0x0001U
#define PORTSTATF_Enable				0x0002U
#define PORTSTATF_Suspend				0x0004U
#define PORTSTATF_Over_Current			0x0008U
#define PORTSTATF_Reset					0x0010U
#define PORTSTATF_Power					0x0100U
#define PORTSTATF_Power_SS				0x0200U		/* USB 3.0 only */
#define PORTSTATF_Full_Speed			0x0000U
#define PORTSTATF_Low_Speed				0x0200U
#define PORTSTATF_High_Speed			0x0400U
#define PORTSTATF_Super_Speed			0x0800U
#define PORTSTATF_Test					0x0800U
#define PORTSTATF_Indicator				0x1000U

// Change Bits
#define PORTCHGF_Connection				0x0001U
#define PORTCHGF_Enable					0x0002U
#define PORTCHGF_Suspend				0x0004U
#define PORTCHGF_Over_Current			0x0008U
#define PORTCHGF_Reset					0x0010U
#define PORTCHGF_PORT_L1				0x0020U		/* USB 2.0 only */
#define PORTCHGF_BH_PORT_RESET			0x0020U		/* USB 3.0 only */
#define PORTCHGF_PORT_LINK_STATE		0x0040U
#define PORTCHGF_PORT_CONFIG_ERROR		0x0080U

/***************************************************************************/

#pragma pack()

/***************************************************************************/

#endif // INC_PRIVATE_HUB_H
