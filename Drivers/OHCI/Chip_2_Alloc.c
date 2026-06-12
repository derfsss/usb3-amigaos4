
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
** 3. All advertising materials mentioning features or use of this software
**    must display the following acknowledgement:
**        This product includes software developed by the NetBSD
**        Foundation, Inc. and its contributors.
** 4. Neither the name of The NetBSD Foundation nor the names of its
**    contributors may be used to endorse or promote products derived
**    from this software without specific prior written permission.
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

// --

#include "usb3_all.h"
#include "OHCI.h"

// --

/* Reverse the bits in a value 0 .. 31 */
static U8 revbits[OHCI_NO_INTRS] =
{
	0x00, 0x10, 0x08, 0x18, 0x04, 0x14, 0x0c, 0x1c,
	0x02, 0x12, 0x0a, 0x1a, 0x06, 0x16, 0x0e, 0x1e,
	0x01, 0x11, 0x09, 0x19, 0x05, 0x15, 0x0d, 0x1d,
	0x03, 0x13, 0x0b, 0x1b, 0x07, 0x17, 0x0f, 0x1f
};

static int Init_HCCABuffer( struct USB3_HCDNode *hn )
{
struct OHCI_HCCA *hcca;
struct OHCI_ED *ped;
struct OHCI_ED *ed;
int retval;
U32 cnt;
U32 phy;
U8 *mem;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Chip_Alloc" );

	retval = FALSE;

	// --

	mem = MEMORY_ALLOC( MEMID_OHCI_HCCA, FALSE );

	if ( ! mem )
	{
		USBERROR( "Init_HCCABuffer : Error allocating memory" );
		goto bailout;
	}

	phy		= ((struct Mem_FreeNode *)mem)->mfn_Addr;
	hcca	= (struct OHCI_HCCA *)mem;

	#ifdef DO_PANIC

	if ( phy == 0 )
	{
		USBPANIC( "Error PhyAddress is Zero (hcca %p)", hcca );
	}

	if ( phy & 0xff )
	{
		USBPANIC( "Bad Alignment 0x%08lx", phy );
	}

	#endif

	hcca->phcca_self_phy = phy;

	hn->hn_HCD.OHCI.HCCA_Table = hcca;

	/* Allocate all the dummy EDs that make up the interrupt tree. */

	for( cnt=0 ; cnt<OHCI_NO_EDS ; cnt++ )
	{
		ed = OHCI_Get_EDBuffer( hn, NULL );

		if ( ! ed )
		{
			USBERROR( "Error allocating memory" );
			goto bailout;
		}

		hn->hn_HCD.OHCI.HCCA_EDArray[cnt] = ed;

		if ( cnt != 0 )
		{
			ped = hn->hn_HCD.OHCI.HCCA_EDArray[ ( cnt - 1 ) / 2 ];
		}
		else
		{
			ped = hn->hn_HCD.OHCI.Isoc_Head;
		}

		ed->ed_nexted 		= LE_SWAP32( ped->ped_self_phy );
		ed->ped_virt_next	= ped;
	}

	/*
	 * Fill HCCA interrupt table.  The bit reversal is to get
	 * the tree set up properly to spread the interrupts.
	 */

	for( cnt=0 ; cnt<OHCI_NO_INTRS ; cnt++ )
	{
		ed = hn->hn_HCD.OHCI.HCCA_EDArray[ OHCI_NO_EDS - OHCI_NO_INTRS + cnt ];

		hn->hn_HCD.OHCI.HCCA_Table->hcca_Interrupt_Table[ revbits[cnt] ] = LE_SWAP32( ed->ped_self_phy );
	}

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --

SEC_CODE S32 OHCI_Chip_Alloc( struct USB3_HCDNode *hn )
{
struct OHCI_ED *ed;
S32 retval;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "OHCI : OHCI_Chip_Alloc" );

	retval = FALSE;

	//-------------

	if ( ! TASK_ALLOCSIGNAL( & hn->hn_HCD.OHCI.Signal_WDH ))
	{
		USBERROR( "Error finding Signal" );
		goto bailout;
	}

	//-------------
	/* Allocate dummy ED that starts the Control list. */

	ed = OHCI_Get_EDBuffer( hn, NULL );

	if ( ! ed )
	{
		USBERROR( "Error allocating memory" );
		goto bailout;
	}

	hn->hn_HCD.OHCI.Ctrl_Head = ed;

	//-------------
	/* Allocate dummy ED that starts the Bulk list. */

	ed = OHCI_Get_EDBuffer( hn, NULL );

	if ( ! ed )
	{
		USBERROR( "Error allocating memory" );
		goto bailout;
	}

	hn->hn_HCD.OHCI.Bulk_Head = ed;

	//-------------
	/* Allocate dummy ED that starts the Isochronous list. */

	ed = OHCI_Get_EDBuffer( hn, NULL );

	if ( ! ed )
	{
		USBERROR( "Error allocating memory" );
		goto bailout;
	}

	hn->hn_HCD.OHCI.Isoc_Head = ed;

	//-------------

	if ( ! Init_HCCABuffer( hn ))
	{
		USBERROR( "HCCA Init Failed" );
		goto bailout;
	}

	//-------------
	// --

	hn->HCD_Mask = hn->hn_HCD.OHCI.Signal_WDH.sig_Signal_Mask;

	// --

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
