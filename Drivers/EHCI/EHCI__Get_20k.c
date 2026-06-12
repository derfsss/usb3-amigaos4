
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
#include "EHCI.h"

// --

SEC_CODE S32 EHCI_Get_20kBuffer( 
	struct USB3_HCDNode *hn, 
	struct EHCI_ITD *itd UNUSED, 
	struct EHCI_TD *td, 
	PTR data, 
	U32 len )
{
U32 phy;
S32 retval;
U8 *mem;

	struct USBBase *usbbase = hn->hn_USBBase;
	TASK_NAME_ENTER( "EHCI : EHCI_Get_20kBuffer" );

	retval = FALSE;

	mem = MEMORY_ALLOC( MEMID_HCD_20k, FALSE );

	if ( ! mem )
	{
		USBERROR( "EHCI_Get_20kBuffer : Error allocating memory" );
		goto bailout;
	}

	phy = ((struct Mem_FreeNode *)mem)->mfn_Addr;

	#ifdef DO_PANIC

	if ( phy == 0 )
	{
		USBPANIC( "Error PhyAddress is Zero ( 20k %p )", mem );
	}

	if ( phy & 0xf )
	{
		USBPANIC( "Bad Alignment 0x%08lx", phy );
	}

	if ( len > 5 * 4096 )
	{
		USBPANIC( "Buffer overflow ( len : %lu )", len );
	}
	
	#endif

	/*
	** Init Transfer Descriptor.
	*/

	if ( td )
	{
		td->td_buffer[0] = LE_SWAP32( phy + 0 * 4096 );
		td->td_buffer[1] = LE_SWAP32( phy + 1 * 4096 );
		td->td_buffer[2] = LE_SWAP32( phy + 2 * 4096 );
		td->td_buffer[3] = LE_SWAP32( phy + 3 * 4096 );
		td->td_buffer[4] = LE_SWAP32( phy + 4 * 4096 );
		td->ptd_buffer_size = len;
		td->ptd_buffer_virt = mem;
		td->ptd_buffer_phy = phy;
	}

	// Send command
	if (( len ) && ( data ))
	{
//		USBDEBUG( "Copy Mem : %p -> %p (%ld)", data, mem, len );
		// copy src, dest, len
		MEM_COPY( data, mem, len );
	}

	retval = TRUE;

bailout:

	TASK_NAME_LEAVE();

	return( retval );
}

// --
