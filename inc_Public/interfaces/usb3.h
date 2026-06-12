
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INTERFACES_USB3_H
#define INTERFACES_USB3_H

/****************************************************************************/

#ifndef DEVICES_USB3_H
#include <devices/usb3.h>
#endif

/****************************************************************************/

struct USB3IFace
{
	struct InterfaceData Data;

	U32								APICALL (*Obtain)(struct USB3IFace *Self);
	U32								APICALL (*Release)(struct USB3IFace *Self);
	void							APICALL (*Expunge)(struct USB3IFace *Self);
	struct Interface *				APICALL (*Clone)(struct USB3IFace *Self);

	U32								APICALL (*USB3_Attr_Get)(struct USB3IFace *Self, USB3_ID id, U32 tag, PTR buffer, U32 buffsersize );
	U32								APICALL (*USB3_Attr_Set)(struct USB3IFace *Self, USB3_ID id, U32 tag, PTR buffer, U32 buffsersize );

	struct USB3_Function *			APICALL (*USB3_Fkt_FindTags)(struct USB3IFace *Self, ... );
	struct USB3_Function *			APICALL (*USB3_Fkt_FindList)(struct USB3IFace *Self, struct TagItem *taglist );

	struct USB3_Interface *			APICALL (*USB3_Ifc_FindTags)(struct USB3IFace *Self, ... );
	struct USB3_Interface *			APICALL (*USB3_Ifc_FindList)(struct USB3IFace *Self, struct TagItem *taglist );
	S32								APICALL (*USB3_Ifc_Claim)(struct USB3IFace *Self, struct USB3_Register *reg, struct USB3_Interface *ih );
	S32								APICALL (*USB3_Ifc_Declaim)(struct USB3IFace *Self, struct USB3_Register *reg, struct USB3_Interface *ih );

	struct USB3_Register *			APICALL (*USB3_Reg_RegisterTags)(struct USB3IFace *Self, ...);
	struct USB3_Register *			APICALL (*USB3_Reg_RegisterList)(struct USB3IFace *Self, struct TagItem *taglist );
	void							APICALL (*USB3_Reg_Unregister)(struct USB3IFace *Self, struct USB3_Register *reg );

	struct USB3_EPResource *		APICALL (*USB3_EPRes_ObtainTags)(struct USB3IFace *Self, struct USB3_Register *reg, ... );
	struct USB3_EPResource *		APICALL (*USB3_EPRes_ObtainList)(struct USB3IFace *Self, struct USB3_Register *reg, struct TagItem taglist );
	void							APICALL (*USB3_EPRes_Release)(struct USB3IFace *Self, struct USB3_EPResource *epr );
	U32								APICALL (*USB3_EPRes_Destall)(struct USB3IFace *Self, struct USB3_EPResource *epr );

	PTR								APICALL (*USB3_Notify_Add)(struct USB3IFace *Self, U32 type, struct MsgPort *mp );
	void							APICALL (*USB3_Notify_Remove)(struct USB3IFace *Self, PTR notify );
};

/***************************************************************************/

struct USB3DriverIFace
{
	struct InterfaceData Data;

	U32								APICALL ( *Obtain )	 ( struct USB3DriverIFace *Self );
	U32								APICALL ( *Release ) ( struct USB3DriverIFace *Self );
	void							APICALL ( *Expunge ) ( struct USB3DriverIFace *Self );
	struct Interface *				APICALL ( *Clone )	 ( struct USB3DriverIFace *Self );
	U32								APICALL ( *Entry )	 ( struct USB3DriverIFace *Self, struct USB3_DriverMessage *msg );
};

/***************************************************************************/

#if 0
#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_EXEC_H
#include <exec/exec.h>
#endif
#ifndef EXEC_INTERFACES_H
#include <exec/interfaces.h>
#endif

#ifndef DEVICES_USB3_H
#include <devices/usb3.h>
#endif

#ifdef __cplusplus
#ifdef __USE_AMIGAOS_NAMESPACE__
namespace AmigaOS {
#endif
extern "C" {
#endif

struct USB3IFace
{
	struct InterfaceData Data;

	U32								APICALL (*Obtain)(struct USB3IFace *Self);
	U32								APICALL (*Release)(struct USB3IFace *Self);
	void							APICALL (*Expunge)(struct USB3IFace *Self);
	struct Interface *				APICALL (*Clone)(struct USB3IFace *Self);
	struct USB3_IORequest *			APICALL (*AllocRequestList)(struct USB3IFace *Self, struct TagItem * tags);
	struct USB3_IORequest *			APICALL (*AllocRequestTags)(struct USB3IFace *Self, ...);
	void							APICALL (*FreeRequest)(struct USB3IFace *Self, struct USB3_IORequest * ioreq);
	U32								APICALL (*DestallEndPoint)(struct USB3IFace *Self, struct USB3_EPResource * epr);
	U32								APICALL (*GetEndPointNr)(struct USB3IFace *Self, struct USB3_EPResource * epr);
	struct USB3_Descriptor *			APICALL (*GetNextDescriptor)(struct USB3IFace *Self, struct USB3_Descriptor * desc);
	U32 APICALL (*GetMemoryInfoList)(struct USB3IFace *Self, U32 nr, struct TagItem * tags);
	U32 APICALL (*GetMemoryInfoTags)(struct USB3IFace *Self, U32 nr, ...);
	U32 APICALL (*Stack_GetAttribute)(struct USB3IFace *Self, uint64 id, U32 tag, PTR buffer, U32 buffsersize);
	U32 APICALL (*Stack_SetAttribute)(struct USB3IFace *Self, uint64 id, PTR storage);
	U32 APICALL (*Status_GetGlobalInfoList)(struct USB3IFace *Self, struct TagItem * tags);
	U32 APICALL (*Status_GetGlobalInfoTags)(struct USB3IFace *Self, ...);
	struct USB3_Interface_Desc * APICALL (*GetNextInterfaceDescriptor)(struct USB3IFace *Self, struct USB3_Register *reg, struct USB3_Interface_Desc * desc);
	U32 APICALL (*ActivateAltInterface)(struct USB3IFace *Self, struct USB3_Register *reg, U32 ifcnr, U32 altnr);
	U32 APICALL (*ReloadSettingsList)(struct USB3IFace *Self, struct TagItem * tags);
	U32 APICALL (*ReloadSettingsTags)(struct USB3IFace *Self, ...);
	U32 APICALL (*SaveSettingsList)(struct USB3IFace *Self, struct TagItem * tags);
	U32 APICALL (*SaveSettingsTags)(struct USB3IFace *Self, ...);
	U32 APICALL (*Shutdown)(struct USB3IFace *Self, U32 flags);
	PTR APICALL (*AllocIOBuffer)(struct USB3IFace *Self, U32 size);
	void APICALL (*FreeIOBuffer)(struct USB3IFace *Self, PTR buf);
};

#ifdef __cplusplus
}
#ifdef __USE_AMIGAOS_NAMESPACE__
}
#endif
#endif





#endif

#endif
