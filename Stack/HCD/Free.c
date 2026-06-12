
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --
// Release all resource we can, for this Struct

SEC_CODE static void __Release( struct USBBase *usbbase UNUSED, struct USB3_HCDNode *hn )
{
struct USB3_TaskNode *tn;

//	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );
	// --

	if ( hn->hn_Task )
	{
		tn = hn->hn_Task;

		if ( tn->tn_Owner == hn )
		{
			TASK_UNLOCK(tn);
			hn->hn_Task = NULL;
			tn->tn_Owner = NULL;
		}
		else
		{
			USBDEBUG( "__Driver_Free : TN    and HN mismatch" );
		}
	}

	// --
//	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );
}

// --

/*
** The Node, should have been Removed from the list header before calling.
*/

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __HCD_Free( struct USBBase *usbbase, struct USB3_HCDNode *hn, STR file UNUSED )

#else

SEC_CODE enum FSTAT __HCD_Free( struct USBBase *usbbase, struct USB3_HCDNode *hn )

#endif

{
enum VSTAT vstat;
enum FSTAT fstat;

	TASK_NAME_ENTER( "__HCD_Free" );

	USBDEBUG( "__HCD_Free               : HN    %p : (%s)", hn, (file)?file:"<NULL>" );

	// --

	if ( HCD_LOCK( hn ) == LSTAT_Okay )
	{
		if ( ! hn->hn_FreeMe )
		{
			USBINFO( "Marking HN %p FreeMe", hn );
			hn->hn_FreeMe = TRUE;
		}

		__Release( usbbase, hn );

		HCD_UNLOCK( hn );
	}

	// --

	SEMAPHORE_OBTAIN( & usbbase->usb_LockSemaphore );

	vstat = HCD_VALID( hn );

	/**/ if ( vstat == VSTAT_Null )
	{
		fstat = FSTAT_Null;
	}
	else if ( vstat == VSTAT_Okay )
	{
		if (( hn->hn_Locks > 0 ) || ( ( hn->hn_Task ) ))
		{
			USBDEBUG( "__HCD_Free               : HN    %p : Node still have %ld locks", hn, hn->hn_Locks );
			fstat = FSTAT_Locked;
		}
		else
		{
			hn->hn_StructID = ID_USB3_FREED;
			fstat = FSTAT_Okay;
		}
	}
	else // ( vstat == VSTAT_Error )
	{
		USBPANIC( "__HCD_Free               : HN    %p : Invalid Pointer", hn );
		fstat = FSTAT_Error;
	}

	SEMAPHORE_RELEASE( & usbbase->usb_LockSemaphore );

	// --

	if ( fstat != FSTAT_Okay )
	{
		goto bailout;
	}

	USBINFO( "__HCD_Free               : Freeing %p : (%s)", hn, (file)?file:"<NULL>" );

	// -- Release the PCIDevice resources we acquired in Controllers_Find.
	// Order matters: free the resource range first (it was queried via
	// pcidev->GetResourceRange), then unlock the device, then drop our
	// reference. The autodoc explicitly states GetResourceRange's return
	// value MUST be released with FreeResourceRange, and any device that
	// was Lock()'d must be Unlock()'d before we stop using it.

	if ( hn->hn_PCIDevice )
	{
		if ( hn->hn_PCIDevResource )
		{
			hn->hn_PCIDevice->FreeResourceRange( hn->hn_PCIDevResource );
			hn->hn_PCIDevResource = NULL;
		}
		if ( hn->hn_PCIDevLock )
		{
			hn->hn_PCIDevice->Unlock();
			hn->hn_PCIDevLock = 0;
		}
		// Release the PCIDevice interface itself (matches FindDeviceTags
		// in Controllers_Find). Per autodoc, the interface returned by
		// FindDevice "must be released with FreeDevice after use".
		usbbase->usb_IPCI->FreeDevice( hn->hn_PCIDevice );
		hn->hn_PCIDevice = NULL;
	}

	// --

	#ifdef DO_PANIC

	if ((( Node_Next( hn )) && ( Node_Next( hn ) != (PTR) 0xAC111111 ))
	||	(( Node_Prev( hn )) && ( Node_Prev( hn ) != (PTR) 0xAC222222 )))
	{
		USBPANIC( "__HCD_Free  : Driver Node have not been removed : Self %p : Next %p : Prev %p : (%s)", hn, Node_Next( hn ), Node_Prev( hn ), file );
	}

	#endif

	// --

	MEMORY_FREE( MEMID_USBHCD, hn, 0 );

bailout:

	TASK_NAME_LEAVE();

	return( fstat );
}

// --
