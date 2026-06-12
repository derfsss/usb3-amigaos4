
#
#      -+- Universal serial bus -+-
# Copyright (c) 2012-2026 by Rene W. Olsen
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

###########################################################################
.PHONY: clean revision install installsdk includes strip
###########################################################################

TARGET		:= USB2Analyzer

CFLAGS		:= -O2
CFLAGS		+= -I.
CFLAGS		+= -I../../inc_Public
CFLAGS		+= -W
CFLAGS		+= -Wall
CFLAGS		+= -MMD
CFLAGS		+= -MP
CFLAGS		+= -mcrt=newlib
# CFLAGS		+= -fanalyzer
CFLAGS		+= -gstabs

# Dont optimize code into memcpy functions
#CFLAGS		+= -fno-builtin

# Put Data into its own section, fixes kickstart loader error
#CFLAGS		+= -ffunction-sections
#CFLAGS		+= -fdata-sections
#CFLAGS		+= -fno-section-anchors 

# We do not want a clib
LDFLAGS		:= -mcrt=newlib

# Use Linker script
#LDFLAGS		+= -T usb2.ld

# Put link data into this file
#LDFLAGS		+= -Wl,--cref,-M,-Map=usb2.map

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

SRCDIRS		+= src
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
#	$(COPY) include/devices/usb3.h		$(SDK)local/common/include/devices
#	$(COPY) include/inline4/usb3.h		$(SDK)local/common/include/inline4
#	$(COPY) include/interfaces/usb3.h	$(SDK)local/common/include/interfaces
#	$(COPY) include/proto/usb3.h		$(SDK)local/common/include/proto

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
