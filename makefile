
#
#      -+- Universal serial bus -+-
# Copyright (c) 2012-2026 by Rene W. Olsen
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

###########################################################################
.PHONY: clean revision install installsdk includes strip
###########################################################################

TARGET		:= usb2.device
VERSION		:= 1
REVISION	:= 9

# Add Drivers 0/1
DRV_MSD		:= 1
DRV_HID		:= 1
DRV_HUB		:= 1
DRV_EHCI	:= 1
DRV_OHCI	:= 1
DRV_UHCI	:= 1
DRV_XHCI	:= 1

CFLAGS		:= -O2
CFLAGS		+= -I.
CFLAGS		+= -Iinc_Public
CFLAGS		+= -Iinc_Private
CFLAGS		+= -W
CFLAGS		+= -Wall
CFLAGS		+= -MMD
CFLAGS		+= -MP
# CFLAGS		+= -fanalyzer

# Dont optimize code into memcpy functions
CFLAGS		+= -fno-builtin

# Put Data into its own section, fixes kickstart loader error
#CFLAGS		+= -ffunction-sections
#CFLAGS		+= -fdata-sections
#CFLAGS		+= -fno-section-anchors 

# We do not want a clib
LDFLAGS		:= -nostartfiles

# Use Linker script
#LDFLAGS		+= -T usb2.ld

# Put link data into this file
LDFLAGS		+= -Wl,--cref,-M,-Map=usb2.map

# Remove unused data .. super strip
#LDFLAGS		+= -Wl,--gc-sections

# We need too tell linker that Resident / .keep section to say
#LDFLAGS		+= -Wl,--undefined=dev_res





#CFLAGS += -m32 -mno-longcall -fno-builtin-llsc

#CFLAGS		+= -Wwrite-strings

#LDFLAGS		+= -Wl,--no-relax

# test .. trying to fix MSD breaking kickstart loader
#CFLAGS		+= -fno-merge-constants
# test

LIBS		:=

SRCS		:=

OBJDIR		:= obj

BINDIR		:= bin

###########################################################################
# Change these as required

#
# For Release those two optional settings..
# or all off, if you want Speed and Small file size
#
# DO_STABLE		adds extra check
# DO_ERROR 		print error messages
#
#
# For Debug
#
# DO_PANIC		too trigger crashes
# DO_ERROR		for Error messages
# DO_INFO		print nice info :)
# DO_DEBUG		for extra info .. is a lot .. so optional
#

# Print Error Messages on serial
#CFLAGS		+= -DDO_ERROR

# Print Info Messages on serial
#CFLAGS		+= -DDO_INFO

# Print extra Debug Messages on serial
#CFLAGS		+= -DDO_DEBUG

# Add extra error code checks, 
# for higher Stability and larger Binary file
#CFLAGS		+= -DDO_STABLE

# Overwrite memory before and after release, 
# scramble pointer on Node Removes and so on
# trying to trigger crashes on error
#CFLAGS		+= -DDO_PANIC

# gcc crash/debug info
#CFLAGS		+= -gstabs

# Enable for Notifycations.. like Ringhio Message, and USBAnalyzer change
#CFLAGS		+= -DDO_NOTIFY

###########################################################################

SRCDIRS		:=

ifeq ($(DRV_MSD),1)
SRCDIRS		+= Drivers/MSD
CFLAGS		+= -DDRV_MSD
endif

ifeq ($(DRV_HUB),1)
SRCDIRS		+= Drivers/HUB
CFLAGS		+= -DDRV_HUB
endif

ifeq ($(DRV_HID),1)
SRCDIRS		+= Drivers/HID
CFLAGS		+= -DDRV_HID
endif

ifeq ($(DRV_EHCI),1)
SRCDIRS		+= Drivers/EHCI
CFLAGS		+= -DDRV_EHCI
endif

ifeq ($(DRV_OHCI),1)
SRCDIRS		+= Drivers/OHCI
CFLAGS		+= -DDRV_OHCI
endif

ifeq ($(DRV_UHCI),1)
SRCDIRS		+= Drivers/UHCI
CFLAGS		+= -DDRV_UHCI
endif

ifeq ($(DRV_XHCI),1)
SRCDIRS		+= Drivers/XHCI
CFLAGS		+= -DDRV_XHCI
endif

###########################################################################

# Lets puts Resident tags first
# Kickstart modules like that
RESSRCS		:= Stack/Device/Device.c
RESSRCS		+= Stack/Device/Fullboot.c		# Fullboot should be after Device.c
#
SRCS		+= Stack/Config/Alloc.c
SRCS		+= Stack/Config/Free.c
SRCS		+= Stack/Config/Lock.c
SRCS		+= Stack/Config/Unlock.c
SRCS		+= Stack/Config/Valid.c
SRCS		+= Stack/Config/Parse.c
SRCS		+= Stack/Config/Set.c
SRCS		+= Stack/Driver/Alloc.c
SRCS		+= Stack/Driver/Free.c
SRCS		+= Stack/Driver/Lock.c
SRCS		+= Stack/Driver/Unlock.c
SRCS		+= Stack/Driver/Valid.c
SRCS		+= Stack/EndPoint/Alloc.c
SRCS		+= Stack/EndPoint/Free.c
SRCS		+= Stack/EndPoint/Lock.c
SRCS		+= Stack/EndPoint/Unlock.c
SRCS		+= Stack/EndPoint/Valid.c
SRCS		+= Stack/EndPointRes/Alloc.c
SRCS		+= Stack/EndPointRes/Free.c
SRCS		+= Stack/EndPointRes/Lock.c
SRCS		+= Stack/EndPointRes/Obtain.c
SRCS		+= Stack/EndPointRes/Release.c
SRCS		+= Stack/EndPointRes/Unlock.c
SRCS		+= Stack/EndPointRes/Valid.c
SRCS		+= Stack/EndPointRes/Destall.c
SRCS		+= Stack/Function/Alloc.c
SRCS		+= Stack/Function/Claim.c
SRCS		+= Stack/Function/Declaim.c
SRCS		+= Stack/Function/Free.c
SRCS		+= Stack/Function/Lock.c
SRCS		+= Stack/Function/Unlock.c
SRCS		+= Stack/Function/Valid.c
SRCS		+= Stack/Function/Detach_Mark.c
#SRCS		+= Stack/Function/Detach_Free.c
SRCS		+= Stack/Function_Init/Init.c
SRCS		+= Stack/Function_Bind/Bind.c
SRCS		+= Stack/FDriver/Driver_Create.c
SRCS		+= Stack/FDriver/Driver_Delete.c
SRCS		+= Stack/FDriver/Driver_Load.c
SRCS		+= Stack/FDriver/Alloc.c
SRCS		+= Stack/FDriver/Free.c
SRCS		+= Stack/FDriver/Lock.c
SRCS		+= Stack/FDriver/Unlock.c
SRCS		+= Stack/FDriver/Valid.c
SRCS		+= Stack/HCD/Add_Request.c
SRCS		+= Stack/HCD/Addr_Obtain.c
SRCS		+= Stack/HCD/Addr_Release.c
SRCS		+= Stack/HCD/Alloc.c
SRCS		+= Stack/HCD/Controllers_Find.c
SRCS		+= Stack/HCD/Controllers_Start.c
SRCS		+= Stack/HCD/Controllers_Stop.c
SRCS		+= Stack/HCD/Free.c
SRCS		+= Stack/HCD/Lock.c
SRCS		+= Stack/HCD/Reply.c
SRCS		+= Stack/HCD/Restart_EndPoint.c
SRCS		+= Stack/HCD/Transfer_Check.c
SRCS		+= Stack/HCD/Transfer_Remove.c
SRCS		+= Stack/HCD/Unlock.c
SRCS		+= Stack/HCD/Valid.c
SRCS		+= Stack/HCD/Wait.c
SRCS		+= Stack/Interface/Group_Alloc.c
SRCS		+= Stack/Interface/Group_Free.c
SRCS		+= Stack/Interface/Group_Lock.c
SRCS		+= Stack/Interface/Group_Unlock.c
SRCS		+= Stack/Interface/Group_Valid.c
SRCS		+= Stack/Interface/Header_Alloc.c
SRCS		+= Stack/Interface/Header_Free.c
SRCS		+= Stack/Interface/Header_Lock.c
SRCS		+= Stack/Interface/Header_Unlock.c
SRCS		+= Stack/Interface/Header_Valid.c
SRCS		+= Stack/Interface/Header_Claim.c
SRCS		+= Stack/Interface/Header_Declaim.c
SRCS		+= Stack/Interface/Node_Alloc.c
SRCS		+= Stack/Interface/Node_Free.c
SRCS		+= Stack/Interface/Node_Lock.c
SRCS		+= Stack/Interface/Node_Unlock.c
SRCS		+= Stack/Interface/Node_Valid.c
SRCS		+= Stack/IORequest/Active_Add.c
SRCS		+= Stack/IORequest/Active_Sub.c
SRCS		+= Stack/IORequest/Alloc.c
SRCS		+= Stack/IORequest/Free.c
SRCS		+= Stack/IORequest/Lock.c
SRCS		+= Stack/IORequest/Unlock.c
SRCS		+= Stack/IORequest/Valid.c
SRCS		+= Stack/Master/Shutdown.c
SRCS		+= Stack/Master/Startup.c
SRCS		+= Stack/Master/Master.c
SRCS		+= Stack/Memory/Alloc.c
SRCS		+= Stack/Memory/Cleanup.c
SRCS		+= Stack/Memory/Free.c
SRCS		+= Stack/Memory/Setup.c
SRCS		+= Stack/Misc/ASync_Add.c
SRCS		+= Stack/Misc/ASync_Free.c
SRCS		+= Stack/Misc/ASync_Init.c
SRCS		+= Stack/Misc/ASync_Sub.c
SRCS		+= Stack/Misc/ASync_Wait.c
SRCS		+= Stack/Misc/Debug_Panic.c
SRCS		+= Stack/Misc/Debug_Error.c
SRCS		+= Stack/Misc/Debug_Debug.c
SRCS		+= Stack/Misc/Debug_Info.c
SRCS		+= Stack/Misc/Debug_Task.c
SRCS		+= Stack/Misc/Desc_Next_Desc.c
SRCS		+= Stack/Misc/Desc_Next_Interface.c
SRCS		+= Stack/Misc/Do_Command.c
SRCS		+= Stack/Misc/DOS_SetCurrentDir.c
SRCS		+= Stack/Misc/DOS_SetProcWindow.c
SRCS		+= Stack/Misc/DOS_OpenFile.c
SRCS		+= Stack/Misc/DOS_CloseFile.c
SRCS		+= Stack/Misc/DOS_GetFileSize.c
SRCS		+= Stack/Misc/DOS_LoadFile.c
SRCS		+= Stack/Misc/DOS_ReadFile.c
SRCS		+= Stack/Misc/EXEC_Device_Add.c
SRCS		+= Stack/Misc/EXEC_Library_Open.c
SRCS		+= Stack/Misc/EXEC_Library_Create.c
SRCS		+= Stack/Misc/EXEC_Library_Close.c
SRCS		+= Stack/Misc/EXEC_Interface_Obtain.c
SRCS		+= Stack/Misc/EXEC_Interface_Release.c
SRCS		+= Stack/Misc/EXEC_Reboot_Cold.c
SRCS		+= Stack/Misc/EXEC_Reboot_IceCold.c
SRCS		+= Stack/Misc/INTUI_GetPrefs.c
SRCS		+= Stack/Misc/Handler_Interrupt_Add.c
SRCS		+= Stack/Misc/Handler_Interrupt_Rem.c
SRCS		+= Stack/Misc/Handler_Reset_Add.c
SRCS		+= Stack/Misc/Handler_Reset_Rem.c
SRCS		+= Stack/Misc/IO_Abort.c
SRCS		+= Stack/Misc/IO_Do.c
SRCS		+= Stack/Misc/IO_Send.c
SRCS		+= Stack/Misc/IO_Stop.c
SRCS		+= Stack/Misc/IO_Restart.c
SRCS		+= Stack/Misc/Mem_AllocIOBuffer.c
SRCS		+= Stack/Misc/Mem_AllocVec.c
SRCS		+= Stack/Misc/Mem_Copy.c
SRCS		+= Stack/Misc/Mem_FreeIOBuffer.c
SRCS		+= Stack/Misc/Mem_FreeVec.c
SRCS		+= Stack/Misc/Mem_PrintF.c
SRCS		+= Stack/Misc/Mem_Set.c
SRCS		+= Stack/Misc/MsgPort_Free.c
SRCS		+= Stack/Misc/MsgPort_GetMsg.c
SRCS		+= Stack/Misc/MsgPort_Init.c
SRCS		+= Stack/Misc/MsgPort_Reinit.c
SRCS		+= Stack/Misc/MsgPort_PutMsg.c
SRCS		+= Stack/Misc/MsgPort_ReplyMsg.c
SRCS		+= Stack/Misc/MsgPort_WaitPort.c
SRCS		+= Stack/Misc/New_Notify_ID.c
SRCS		+= Stack/Misc/Node_AddHead.c
SRCS		+= Stack/Misc/Node_AddTail.c
SRCS		+= Stack/Misc/Node_AddInfronOf.c
SRCS		+= Stack/Misc/Node_RemHead.c
SRCS		+= Stack/Misc/Node_RemNode.c
SRCS		+= Stack/Misc/Node_Next.c
SRCS		+= Stack/Misc/Node_Prev.c
SRCS		+= Stack/Misc/SetupData_Alloc.c
SRCS		+= Stack/Misc/SetupData_Free.c
SRCS		+= Stack/Misc/Semaphore_Attempt.c
SRCS		+= Stack/Misc/Semaphore_Free.c
SRCS		+= Stack/Misc/Semaphore_Init.c
SRCS		+= Stack/Misc/Semaphore_Obtain.c
SRCS		+= Stack/Misc/Semaphore_Release.c
SRCS		+= Stack/Misc/Signal_Alloc.c
SRCS		+= Stack/Misc/Signal_Realloc.c
SRCS		+= Stack/Misc/Signal_Free.c
SRCS		+= Stack/Misc/Signal_Set.c
SRCS		+= Stack/Misc/Signal_Signal.c
SRCS		+= Stack/Misc/Signal_Wait.c
SRCS		+= Stack/Misc/StrCaseCompare.c
SRCS		+= Stack/Misc/StrICaseCompare.c
SRCS		+= Stack/Misc/StrINCaseCompare.c
SRCS		+= Stack/Misc/StrLength.c
SRCS		+= Stack/Misc/Tag_GetData.c
SRCS		+= Stack/Misc/Tag_GetValue.c
SRCS		+= Stack/Misc/Tag_NextTagItem.c
SRCS		+= Stack/Misc/TaskNode.c
SRCS		+= Stack/Misc/Task_Find.c
SRCS		+= Stack/Misc/Task_SetPri.c
SRCS		+= Stack/Misc/Wait.c
SRCS		+= Stack/Misc/MicroDelay.c
SRCS		+= Stack/TaskMsg/Alloc.c
SRCS		+= Stack/TaskMsg/Free.c
SRCS		+= Stack/TaskMsg/Lock.c
SRCS		+= Stack/TaskMsg/Unlock.c
SRCS		+= Stack/TaskMsg/Valid.c
SRCS		+= Stack/Register/Alloc.c
SRCS		+= Stack/Register/Free.c
SRCS		+= Stack/Register/Lock.c
SRCS		+= Stack/Register/Register.c
SRCS		+= Stack/Register/Unlock.c
SRCS		+= Stack/Register/Unregister.c
SRCS		+= Stack/Register/Valid.c
SRCS		+= Stack/RootHUB/Handle_Read.c
SRCS		+= Stack/RootHUB/Handle_Write.c
SRCS		+= Stack/RootHUB/Handle_Change.c
SRCS		+= Stack/Task/Alloc.c
SRCS		+= Stack/Task/Free.c
SRCS		+= Stack/Task/Lock.c
SRCS		+= Stack/Task/Misc.c
SRCS		+= Stack/Task/Start.c
SRCS		+= Stack/Task/Stop_TN.c
SRCS		+= Stack/Task/Stop_DN.c
SRCS		+= Stack/Task/Stop_FN.c
SRCS		+= Stack/Task/Unlock.c
SRCS		+= Stack/Task/Valid.c

SRCS		+= $(wildcard $(addsuffix /*.c, $(SRCDIRS)))
SRCS		:= $(sort $(SRCS))

###########################################################################

HOST_OS		:= $(shell uname)

# --

ifeq ($(HOST_OS),AmigaOS)

LS			:= list
RM			:= delete
CC			:= gcc
STRIP		:= strip
MKDIR		:= makedir FORCE
COPY		:= copy
SDK			:= sdk:

# --

else ifeq ($(HOST_OS),Linux)

LS			:= ls -lort
RM			:= rm -rf
CC			:= ppc-amigaos-gcc
STRIP		:= ppc-amigaos-strip
MKDIR		:= mkdir -p
COPY		:= cp
SDK			:= /SDK/

endif

###########################################################################

SRCS2		:= $(RESSRCS)
SRCS2		+= $(SRCS)
OBJS		:= $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS2))
DEPS		:= $(patsubst %.c,$(OBJDIR)/%.d,$(SRCS2))

all: $(BINDIR)/$(TARGET)

# Static analysis build — run: make analyze
analyze: CFLAGS += -fanalyzer
analyze: all

###########################################################################

$(BINDIR)/$(TARGET): $(OBJS) | $(BINDIR)
	@echo "Linking $(BINDIR)/$(TARGET)"
	@$(CC) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(OBJS) $(LIBS)

$(BINDIR):
	$(MKDIR) $(BINDIR)

info:
	ppc-amigaos-size $(BINDIR)/$(TARGET)
	ppc-amigaos-objdump -h $(BINDIR)/$(TARGET) | egrep '\.text|\.data|\.rodata|\.bss'

###########################################################################

# Include dependency files
-include $(DEPS)

###########################################################################

clean:
	$(RM) $(OBJDIR) $(DEPS) $(BINDIR)/$(TARGET) usb2.map

###########################################################################

#revision:
#	bumprev -v $(VERSION) -r $(REVISION) -n $(TARGET) -b $(TARGET)

###########################################################################

install:
	copy $(BINDIR)/usb2.device sys:kickstart/

###########################################################################

#installsdk:
#	$(MKDIR) $(SDK)local/common/include/devices
#	$(MKDIR) $(SDK)local/common/include/inline4
#	$(MKDIR) $(SDK)local/common/include/interfaces
#	$(MKDIR) $(SDK)local/common/include/proto
#	$(COPY) include/devices/usb2.h		$(SDK)local/common/include/devices
#	$(COPY) include/inline4/usb2.h		$(SDK)local/common/include/inline4
#	$(COPY) include/interfaces/usb2.h	$(SDK)local/common/include/interfaces
#	$(COPY) include/proto/usb2.h		$(SDK)local/common/include/proto

###########################################################################

strip:
	@$(LS)		$(BINDIR)/$(TARGET)
	@$(STRIP)	$(BINDIR)/$(TARGET)
	@$(LS)		$(BINDIR)/$(TARGET)

###########################################################################

$(OBJDIR)/%.o: %.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

###########################################################################
# Put all drivers into its own section

$(OBJDIR)/Drivers/%.o: Drivers/%.c
	@$(MKDIR) $(dir $@)
	@echo "Compiling Driver : $<"
	@$(CC) $(CFLAGS) -c $< -o $@

#	@$(CC) $(CFLAGS) -DSECTION_NAME=\"drivers\" -c $< -o $@

###########################################################################
# Put the stack into its own section

$(OBJDIR)/Stack/%.o: Stack/%.c
	@$(MKDIR) $(dir $@)
	@echo "Compiling Stack : $<"
	@$(CC) $(CFLAGS) -c $< -o $@

###########################################################################
