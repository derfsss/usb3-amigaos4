
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

#define USB2_Attr_Get(x,y,z,q)				IUSB2->USB2_Attr_Get((x),(y),(z),(q))
#define USB2_Attr_Set(x,y,z,q)				IUSB2->USB2_Attr_Set((x),(y),(z),(q))

#define USB2_Fkt_FindTags(...)				IUSB2->USB2_Fkt_FindTags(__VA_ARGS__)
#define USB2_Fkt_FindList(x)				IUSB2->USB2_Fkt_FindList((x))

#define USB2_Ifc_FindTags(...)				IUSB2->USB2_Ifc_FindTags(__VA_ARGS__)
#define USB2_Ifc_FindList(x)				IUSB2->USB2_Ifc_FindList((x))
#define USB2_Ifc_Claim(x,y)					IUSB2->USB2_Ifc_Claim((x),(y))
#define USB2_Ifc_Declaim(x,y)				IUSB2->USB2_Ifc_Declaim((x),(y))

#define USB2_Reg_RegisterTags(...)			IUSB2->USB2_Reg_RegisterTags(__VA_ARGS__)
#define USB2_Reg_RegisterList(x)			IUSB2->USB2_Reg_RegisterList((x))
#define USB2_Reg_Unregister(x)				IUSB2->USB2_Reg_Unregister((x))

#define USB2_EPRes_ObtainTags(...)			IUSB2->USB2_EPRes_ObtainTags(__VA_ARGS__)
#define USB2_EPRes_ObtainList(x,y)			IUSB2->USB2_EPRes_ObtainList((x),(y))
#define USB2_EPRes_Release(x)				IUSB2->USB2_EPRes_Release((x))
#define USB2_EPRes_Destall(x)				IUSB2->USB2_EPRes_Destall((x))

#define USB2_Notify_Add(x,y)				IUSB2->USB2_Notify_Add((x),(y))
#define USB2_Notify_Remove(x)				IUSB2->USB2_Notify_Remove((x))

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
#define RegisterList(tags) IUSB2->RegisterList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define RegisterTags(...) IUSB2->RegisterTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define RegisterTags(...) IUSB2->RegisterTags(## vargs) 
#endif
#define Unregister( reg ) IUSB2->Unregister(( reg )) 
#define AllocRequestList(tags) IUSB2->AllocRequestList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define AllocRequestTags(...) IUSB2->AllocRequestTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define AllocRequestTags(...) IUSB2->AllocRequestTags(## vargs) 
#endif
#define FreeRequest(ioreq) IUSB2->FreeRequest((ioreq)) 
#define DestallEndPoint(epr) IUSB2->DestallEndPoint((epr)) 
#define GetEndPointNr(epr) IUSB2->GetEndPointNr((epr)) 
#define GetNextDescriptor(desc) IUSB2->GetNextDescriptor((desc)) 
#define AddNotify(type, mp) IUSB2->AddNotify((type), (mp)) 
#define RemNotify(notify) IUSB2->RemNotify((notify)) 
#define GetMemoryInfoList(nr, tags) IUSB2->GetMemoryInfoList((nr), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define GetMemoryInfoTags(...) IUSB2->GetMemoryInfoTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define GetMemoryInfoTags(vargs...) IUSB2->GetMemoryInfoTags(## vargs) 
#endif
#define Stack_GetAttribute(id, tag, buffer, buffsersize) IUSB2->Stack_GetAttribute((id), (tag), (buffer), (buffsersize)) 
#define Stack_SetAttribute(id, storage) IUSB2->Stack_SetAttribute((id), (storage)) 
#define Status_GetGlobalInfoList(tags) IUSB2->Status_GetGlobalInfoList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define Status_GetGlobalInfoTags(...) IUSB2->Status_GetGlobalInfoTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define Status_GetGlobalInfoTags(...) IUSB2->Status_GetGlobalInfoTags(## vargs) 
#endif
#define GetNextInterfaceDescriptor(reg, desc) IUSB2->GetNextInterfaceDescriptor(( reg ), (desc)) 
#define ActivateAltInterface(reg, ifcnr, altnr) IUSB2->ActivateAltInterface(( reg ), (ifcnr), (altnr)) 
#define ReloadSettingsList(tags) IUSB2->ReloadSettingsList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define ReloadSettingsTags(...) IUSB2->ReloadSettingsTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define ReloadSettingsTags(...) IUSB2->ReloadSettingsTags(## vargs) 
#endif
#define SaveSettingsList(tags) IUSB2->SaveSettingsList((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define SaveSettingsTags(...) IUSB2->SaveSettingsTags(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define SaveSettingsTags(...) IUSB2->SaveSettingsTags(## vargs) 
#endif
#define Shutdown(flags) IUSB2->Shutdown((flags)) 
#define AllocIOBuffer(size) IUSB2->AllocIOBuffer((size)) 
#define FreeIOBuffer(buf) IUSB2->FreeIOBuffer((buf)) 
	#endif

#endif /* INLINE4_USB3_H */
