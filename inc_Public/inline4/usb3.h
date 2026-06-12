
/*
**      -+- Universal serial bus -+-
** Copyright (c) 2012-2026 by Rene W. Olsen
**
** SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INLINE4_USB3_H
#define INLINE4_USB3_H

/****************************************************************************/

#ifndef DEVICES_USB3_H
#include <devices/usb3.h>
#endif

/****************************************************************************/

#define USB3_Attr_Get(x,y,z,q)				IUSB3->USB3_Attr_Get((x),(y),(z),(q))
#define USB3_Attr_Set(x,y,z,q)				IUSB3->USB3_Attr_Set((x),(y),(z),(q))

#define USB3_Fkt_FindTags(...)				IUSB3->USB3_Fkt_FindTags(__VA_ARGS__)
#define USB3_Fkt_FindList(x)				IUSB3->USB3_Fkt_FindList((x))

#define USB3_Ifc_FindTags(...)				IUSB3->USB3_Ifc_FindTags(__VA_ARGS__)
#define USB3_Ifc_FindList(x)				IUSB3->USB3_Ifc_FindList((x))
#define USB3_Ifc_Claim(x,y)					IUSB3->USB3_Ifc_Claim((x),(y))
#define USB3_Ifc_Declaim(x,y)				IUSB3->USB3_Ifc_Declaim((x),(y))

#define USB3_Reg_RegisterTags(...)			IUSB3->USB3_Reg_RegisterTags(__VA_ARGS__)
#define USB3_Reg_RegisterList(x)			IUSB3->USB3_Reg_RegisterList((x))
#define USB3_Reg_Unregister(x)				IUSB3->USB3_Reg_Unregister((x))

#define USB3_EPRes_ObtainTags(...)			IUSB3->USB3_EPRes_ObtainTags(__VA_ARGS__)
#define USB3_EPRes_ObtainList(x,y)			IUSB3->USB3_EPRes_ObtainList((x),(y))
#define USB3_EPRes_Release(x)				IUSB3->USB3_EPRes_Release((x))
#define USB3_EPRes_Destall(x)				IUSB3->USB3_EPRes_Destall((x))

#define USB3_Notify_Add(x,y)				IUSB3->USB3_Notify_Add((x),(y))
#define USB3_Notify_Remove(x)				IUSB3->USB3_Notify_Remove((x))

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
#include <interfaces/usb3.h>

/* Inline macros for Interface "main" */
#define RegisterList(tags) IUSB3->RegisterList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define RegisterTags(...) IUSB3->RegisterTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define RegisterTags(...) IUSB3->RegisterTags(## vargs) 
#endif
#define Unregister( reg ) IUSB3->Unregister(( reg )) 
#define AllocRequestList(tags) IUSB3->AllocRequestList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define AllocRequestTags(...) IUSB3->AllocRequestTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define AllocRequestTags(...) IUSB3->AllocRequestTags(## vargs) 
#endif
#define FreeRequest(ioreq) IUSB3->FreeRequest((ioreq)) 
#define DestallEndPoint(epr) IUSB3->DestallEndPoint((epr)) 
#define GetEndPointNr(epr) IUSB3->GetEndPointNr((epr)) 
#define GetNextDescriptor(desc) IUSB3->GetNextDescriptor((desc)) 
#define AddNotify(type, mp) IUSB3->AddNotify((type), (mp)) 
#define RemNotify(notify) IUSB3->RemNotify((notify)) 
#define GetMemoryInfoList(nr, tags) IUSB3->GetMemoryInfoList((nr), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define GetMemoryInfoTags(...) IUSB3->GetMemoryInfoTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define GetMemoryInfoTags(vargs...) IUSB3->GetMemoryInfoTags(## vargs) 
#endif
#define Stack_GetAttribute(id, tag, buffer, buffsersize) IUSB3->Stack_GetAttribute((id), (tag), (buffer), (buffsersize)) 
#define Stack_SetAttribute(id, storage) IUSB3->Stack_SetAttribute((id), (storage)) 
#define Status_GetGlobalInfoList(tags) IUSB3->Status_GetGlobalInfoList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define Status_GetGlobalInfoTags(...) IUSB3->Status_GetGlobalInfoTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define Status_GetGlobalInfoTags(...) IUSB3->Status_GetGlobalInfoTags(## vargs) 
#endif
#define GetNextInterfaceDescriptor(reg, desc) IUSB3->GetNextInterfaceDescriptor(( reg ), (desc)) 
#define ActivateAltInterface(reg, ifcnr, altnr) IUSB3->ActivateAltInterface(( reg ), (ifcnr), (altnr)) 
#define ReloadSettingsList(tags) IUSB3->ReloadSettingsList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define ReloadSettingsTags(...) IUSB3->ReloadSettingsTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define ReloadSettingsTags(...) IUSB3->ReloadSettingsTags(## vargs) 
#endif
#define SaveSettingsList(tags) IUSB3->SaveSettingsList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define SaveSettingsTags(...) IUSB3->SaveSettingsTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define SaveSettingsTags(...) IUSB3->SaveSettingsTags(## vargs) 
#endif
#define Shutdown(flags) IUSB3->Shutdown((flags)) 
#define AllocIOBuffer(size) IUSB3->AllocIOBuffer((size)) 
#define FreeIOBuffer(buf) IUSB3->FreeIOBuffer((buf)) 
	#endif

#endif /* INLINE4_USB3_H */
