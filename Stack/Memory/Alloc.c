
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

SEC_CODE static inline void __Add_non_Aligned( struct USBBase *usbbase, struct Mem_HeaderNode *h )
{
struct Mem_FreeNode *mem;

	mem = MEM_ALLOCVEC( h->mhn_Mem_Size, FALSE );

	if ( mem )
	{
		// Init Mem Node
		mem->mfn_Next = h->mhn_Free_Nodes;
		mem->mfn_Addr = 0;

		// Update Stats
		h->mhn_Free_Nodes = mem;
		h->Mem.Normal.Free_Count++;
		h->Mem.Normal.Total_Count++;
	}
}

// --

SEC_CODE static inline void __Add_Aligned( struct USBBase *usbbase, struct Mem_HeaderNode *h )
{
struct Mem_PageNode *mpn;
struct Mem_FreeNode *tmp;
struct ExecIFace *IExec;
struct MMUIFace *IMMU;
U32 rsize;	// Real Buffer Size
PTR stack;
U32 size;	// Chunk Size (inside buffer)
U32 loop;
PTR mem;
U32 phy;
U32 cnt;

	IMMU  = usbbase->usb_IMMU;
	IExec = usbbase->usb_IExec;
	stack = IExec->SuperState();
	rsize = h->Mem.Align.Real_Size;

//	USBDEBUG( "AddMem: Realsize %lu", rsize );

	#ifdef DO_DEBUG

	if ( rsize & 0x00000FFFUL )
	{
		USBPANIC( "__Add_Aligned : Invalid Real Size $%08lx (%lu)", rsize, rsize );
	}

	#endif

	// --
	// Memory Aligned Allocations

	mpn = MEM_ALLOCVEC( sizeof( struct Mem_PageNode ), TRUE );

//	USBDEBUG( "AddMem: mpn %p", mpn );

	if ( ! mpn )
	{
		USBDEBUG( "__Add_Aligned : 1 : Error allocating memory" );
		goto bailout;
	}

	// --

	mem = IExec->AllocVecTags( rsize,
		AVT_PhysicalAlignment, usbbase->usb_CPU_PageSize,
		AVT_Alignment, usbbase->usb_CPU_PageSize,
		AVT_Type, MEMF_SHARED,
		AVT_Contiguous, TRUE,
		AVT_Lock, TRUE,
		TAG_END
	);

//	USBDEBUG( "AddMem: mem %p", mem );

	if ( ! mem )
	{
		USBERROR( "__Add_Aligned : 2 : Error allocating memory" );
		MEM_FREEVEC( mpn );
		goto bailout;
	}

	// -- Link Page, so its tracked

//	USBDEBUG( "AddMem: AddTail" );

	NODE_ADDTAIL( & h->Mem.Align.Pages, mpn );
	mpn->mpn_Memory = mem;

	// -- Disable Cahce for Memory page(s) and get real Physical Address

//	USBDEBUG( "AddMem: Clear Cache %p:%08lx", mem, rsize-1 );

	IExec->CacheClearE(	mem, rsize, CACRF_ClearD | CACRF_ClearI );

//	USBDEBUG( "AddMem: Set Page Flags %p:%08lx", mem, rsize-1 );

	IMMU->SetMemoryAttrs( mem, rsize, MEMATTRF_READ_WRITE|MEMATTRF_CACHEINHIBIT );

//	USBDEBUG( "AddMem: Invalite Cache" );

	IExec->CacheClearE(	mem, rsize, CACRF_InvalidateD );

//	USBDEBUG( "AddMem: Get Phy" );

	phy = (U32) IMMU->GetPhysicalAddress( mem );

//	USBDEBUG( "AddMem: phy $%08lx", phy );

	// --

	size = (( h->mhn_Mem_Size + h->mhn_Align - 1 ) / h->mhn_Align ) * h->mhn_Align;

//	USBDEBUG( "AddMem: size %lu", size );

	loop = rsize / size;

	mpn->mpn_Total = loop;
	mpn->mpn_Page_Adr_1 = phy;
	mpn->mpn_Page_Adr_2 = phy + rsize;

//	USBDEBUG( "AddMem: loops %lu", loop );

	// -- Insert Nodes into Header

	for( cnt=0 ; cnt<loop ; cnt++ )
	{
		tmp = (PTR) ( ((U32)mem) + ( size * cnt ) );
		tmp->mfn_Next = h->mhn_Free_Nodes;
		tmp->mfn_Addr = phy + ( size * cnt );

		// This is slow, and should not be needed if AVT_Contiguous is honored
//		tmp->mfm_Addr = (U32) IMMU->GetPhysicalAddress( tmp );

//		USBDEBUG( "AddMem: cnt %lu, tmp $%08lx, phy $%08lx, Calc Phy $%08lx", cnt, tmp, tmp->mfm_Addr, phy + ( size * cnt ) );

		// Throw away 'Phy 0' Node

		if ( tmp->mfn_Addr )
		{
			h->mhn_Free_Nodes = tmp;
			mpn->mpn_Free++;
		}
		#ifdef DO_DEBUG
		else
		{
			USBDEBUG( "__Add_Aligned : 3 : Throwing away 'Phy 0' Node" );
		}
		#endif
	}

bailout:

	IExec->UserState( stack );

	return;
}

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE PTR __Memory_Alloc( struct USBBase *usbbase, U32 memtype, S32 clear, STR file UNUSED )

#else

SEC_CODE PTR __Memory_Alloc( struct USBBase *usbbase, U32 memtype, S32 clear )

#endif
{
struct Mem_HeaderNode *h;
struct Mem_FreeNode *mem;
struct Mem_PageNode *p;
U32 adr;

	mem = NULL;

	#ifdef DO_DEBUG

	if ( memtype >= MEMID_Last )
	{
		USBDEBUG( "__Memory_Alloc : Illegal Memory Type %ld", memtype );
		goto bailout;
	}

	#endif 

	// --

	h = usbbase->usb_Memory_Headers;
	h = & h[memtype] ;

	SEMAPHORE_OBTAIN( & h->mhn_Semaphore );

	mem = h->mhn_Free_Nodes;

	if ( ! mem )
	{
		// Add Memory

		if ( h->mhn_Align > 1 )
		{
			// do alignment
			__Add_Aligned( usbbase, h );
		}
		else
		{
			// not aligned
			__Add_non_Aligned( usbbase, h );
		}

		mem = h->mhn_Free_Nodes;
	}

	if ( mem )
	{
		if ( h->mhn_Align <= 1 )
		{
			// not aligned
			h->Mem.Normal.Free_Count--;

			#ifdef DO_DEBUG

			if ( h->Mem.Normal.Free_Count < 0 )
			{
				USBDEBUG( "__Memory_Alloc : 1 : Error Negative Free Count (%ld)", h->Mem.Normal.Free_Count );
			}

			#endif
		}
		else
		{
			p = h->Mem.Align.Pages.uh_Head;

			adr = ((struct Mem_FreeNode *)mem)->mfn_Addr;

			while( p )
			{
				if (( p->mpn_Page_Adr_1 <= adr ) && ( adr < p->mpn_Page_Adr_2 ))
				{
					p->mpn_Free--;
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
				USBDEBUG( "__Memory_Alloc : 2 : Page not found" );
			}
			else if ( p->mpn_Free < 0 )
			{
				USBDEBUG( "__Memory_Alloc : 3 : Error Negative Free Count (%ld)", p->mpn_Free );
			}

			#endif
		}

		h->mhn_Free_Nodes = mem->mfn_Next;
	}

	SEMAPHORE_RELEASE( & h->mhn_Semaphore );

	if (( mem ) && ( clear ))
	{
		// This may be slow, so clear outside lock
		MEM_SET( mem, 0, h->mhn_Mem_Size );
	}

	#ifdef DO_DEBUG

bailout:

	{
//		char *_name = ( memtype < MEMID_Last ) ? idnames[memtype] : "Type";
//		USBDEBUG( "Alloc Mem                : Mem %p : %s (%ld) : Clear %ld : (%s) :", mem, _name, memtype, clear, (file)?file:"<NULL>" );
	}

	#endif

	return( mem );
}

// --
