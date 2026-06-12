
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

// --

#include "usb3_all.h"

// --

#if defined( DO_PANIC ) || defined( DO_ERROR ) || defined( DO_DEBUG ) || defined( DO_INFO )

SEC_CODE enum FSTAT __EndPointRes_Release( struct USBBase *usbbase, struct USB3_EPResource *epr, STR file UNUSED )

#else

SEC_CODE enum FSTAT __EndPointRes_Release( struct USBBase *usbbase, struct USB3_EPResource *epr )

#endif

{
struct RealEndPointResource *real;
struct RealRegister *reg;
enum FSTAT stat;

	USBDEBUG( "__EndPointRes_Release : EPR   %p : (%s)", epr, (file)?file:"<NULL>" );

	if ( ! epr )
	{
		stat = FSTAT_Null;
	}
	else
	{
		real = (PTR) ( (char *) epr - offsetof( struct RealEndPointResource, epr_Public ));

		if ( ENDPOINTRES_VALID(real) == VSTAT_Okay )
		{
			reg = real->epr_Register;

			if ( reg )
			{
				USBDEBUG( "--> : REM 2 : EPR   %p to REG %p : Title '%s' :", epr, reg, reg->reg_Title );
				NODE_REMNODE( & reg->reg_EPRHeader, & real->epr_Node );
			}

			stat = ENDPOINTRES_FREE( real );

			if (( reg ) && ( stat != FSTAT_Okay ))
			{
				USBDEBUG( "--> : ADD 3 :EPR %p to REG %p : Title '%s' :", epr, reg, reg->reg_Title );
				NODE_ADDHEAD( & reg->reg_EPRHeader, & real->epr_Node );
			}
			else
			{
				if ( reg->reg_Public.Res_Control == & real->epr_Public )
				{
					USBDEBUG( "__Register_Free          : Found Control EPR (2)" );
					reg->reg_Public.Res_Control = NULL;
				}
			}
		}
		else
		{
			#ifdef DO_DEBUG
			USBPANIC( "__EndPointRes_Release : Invalid EPR : %p -> %p", epr, real );
			#endif
			stat = FSTAT_Null;
		}
	}

	return( stat );
}

// --
