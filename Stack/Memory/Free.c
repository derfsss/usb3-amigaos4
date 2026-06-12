
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

extern char *idnames[MEMID_Last];

// --

SEC_CODE static inline void __Free_non_Aligned( struct USBBase *usbbase, struct Mem_HeaderNode *h, struct Mem_FreeNode *mem, U32 phyadr )
{
	if ( h->Mem.Normal.Free_Count >= h->Mem.Normal.Free_Max )
	{
		// Free Memory

		// Release memory to system
		MEM_FREEVEC( mem );

		// Update Stats
		h->Mem.Normal.Total_Count--;
	}
	else
	{
		// Queue Memory

		// Update Stats
		h->Mem.Normal.Free_Count++;

		// Init Mem Node
		mem->mfn_Next = h->mhn_Free_Nodes;
		mem->mfn_Addr = phyadr;

		// Link
		h->mhn_Free_Nodes = mem;
	}
}

// --

SEC_CODE static inline void __Free_Aligned( struct USBBase *usbbase UNUSED, struct Mem_HeaderNode *h, struct Mem_FreeNode *mem, U32 phyadr )
{
struct Mem_PageNode *p;

	// Queue Memory

	// Update Stats

	p = h->Mem.Align.Pages.uh_Head;

	while( p )
	{
		if (( p->mpn_Page_Adr_1 <= phyadr ) && ( phyadr < p->mpn_Page_Adr_2 ))
		{
			p->mpn_Free++;
			break;
		}
		else
		{
			p = Node_Next(p);
		}
	}

	#ifdef DO_DEBUG

	if ( ! p )
	{
		USBDEBUG( "__Free_Aligned : Page not found" );
	}

	#endif

	// Init Mem Node
	mem->mfn_Next = h->mhn_Free_Nodes;
	mem->mfn_Addr = phyadr;

	// Link
	h->mhn_Free_Nodes = mem;
}

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE void __Memory_Free( struct USBBase *usbbase, U32 memtype, PTR mem, U32 phyadr, STR file UNUSED )

#else

SEC_CODE void __Memory_Free( struct USBBase *usbbase, U32 memtype, PTR mem, U32 phyadr )

#endif
{
struct Mem_HeaderNode *h;

	#if 0
	#ifdef DO_DEBUG

	STR memname = ( memtype < MEMID_Last ) ? idnames[memtype] : "Type";
	USBDEBUG( "Free Mem                 : mem %p : %s (%ld) : (%s)", mem, memname, memtype, (file)?file:"<NULL>" );

	#endif
	#endif

	if ( mem )
	{
		h = usbbase->usb_Memory_Headers;
		h = & h[memtype] ;

		// --

		#ifdef DO_PANIC

		if ( memtype >= MEMID_Last )
		{
			USBPANIC( "Illegal Memory Type %ld", memtype );
		}

		// --

//		if ( memtype == MEMID_USBFunction )
//		{
//			static int hhhuu = 0x60;
//			MEM_SET( mem, hhhuu++, h->mhn_Mem_Size );
//			MEM_SET( mem, 0x60 + memtype, h->mhn_Mem_Size );
//		}
//		else
		{
			// We check for 0x55555555 pointers
			MEM_SET( mem, 0x55, h->mhn_Mem_Size );
		}		

		#endif

		// --

		SEMAPHORE_OBTAIN( & h->mhn_Semaphore );

		if ( h->mhn_Align > 1 )
		{
			// do alignment
//			#warning fix me -- this is just a test
			__Free_Aligned( usbbase, h, mem, phyadr );
		}
		else
		{
			// not aligned
//			#warning fix me -- this is just a test
			__Free_non_Aligned( usbbase, h, mem, phyadr );
	}

		SEMAPHORE_RELEASE( & h->mhn_Semaphore );
	}
}

// --
