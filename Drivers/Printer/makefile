
###########################################################################

.PHONY: all clean revision install makedirs

###########################################################################
#
# Compile info:
#
# Compile : make USBTYPE=SIRION
#
# Options:
#
#  USBTYPE = Sirion (AOS4), USB3
#  USBTYPE - 'SIRION' 'USB3'		-- Target System
#

VERSION		:= 1
REVISION	:= 1

# Default
USBTYPE		?= USB3

# Object Dir
OBJDIR		:= obj

# Binary Dir
BINDIR		:= bin

# Install Dir
INSTALLDIR	:= devs:

# Compiler Flags
CFLAGS		:= -O2
CFLAGS		+= -I.
CFLAGS		+= -I../../inc_Public
CFLAGS		+= -Iinc
CFLAGS		+= -gstabs
CFLAGS		+= -W
CFLAGS		+= -Wall
CFLAGS		+= -MMD
CFLAGS		+= -MP
CFLAGS		+= -mcrt=newlib

# Print Error Messages on serial
#CFLAGS		+= -DDO_ERROR

# Print Info Messages on serial
#CFLAGS		+= -DDO_INFO

# Print Debug Messages on serial
#CFLAGS		+= -DDO_DEBUG

# Linker Flags
LDFLAGS		:= -mcrt=newlib

# Link Libraries
LIBS		:=

# Empty Source File List (Will grow)
SRCS		:=

# Host SYS we compile on
HOSTOS		:= $(shell uname)

###########################################################################

# --

ifeq ($(HOSTOS),AmigaOS)

LS			:= list
RM			:= delete
RMDIR		:= rm -rf
MKDIR		:= makedir ALL

# --

else ifeq ($(HOSTOS),Linux)

LS			:= ls -lort
RM			:= rm
RMDIR		:= rm -rf
MKDIR		:= mkdir -p

# --

else

$(error Unknown Host SYS type: $(HOSTOS).)

endif

###########################################################################
# Set Target USB System
# -- Sirion

ifeq ($(USBTYPE),SIRION)

  $(info Compiling for Sirion)

  SRCS		+= Sirion/Printer.c
  SRCS		+= Sirion/Resources_Init.c
  SRCS		+= Sirion/Resources_Free.c
  SRCS		+= Sirion/Process_Init_2_Find_Device.c
  SRCS		+= Sirion/Process_Init_3_Setup_Device.c
  SRCS		+= Sirion/Process_Main.c

  CFLAGS	+= -ISirion
  CFLAGS	+= -D_SIRION_

  LDFLAGS	:= -nostartfiles
  LDFLAGS	+= -mcrt=newlib

  OBJDIR	:= $(OBJDIR)/Sirion
  TARGET	:= rusb_printer.device

###########################################################################
# Set Target USB System
# -- USB3

else ifeq ($(USBTYPE),USB3)

  $(info Compiling for USB3)

  SRCS		+= USB3/Printer.c
  SRCS		+= USB3/Resources_Init.c
  SRCS		+= USB3/Resources_Free.c
  SRCS		+= USB3/Process_Init_2_Find_Device.c
  SRCS		+= USB3/Process_Init_3_Setup_Device.c
  SRCS		+= USB3/Process_Main.c
  SRCS		+= USB3/Buffer_Schedule.c
  SRCS		+= USB3/Direct_Schedule.c
  SRCS		+= USB3/Unit_Startup_USB.c

  CFLAGS	+= -IUSB3
  CFLAGS	+= -D_USB3_
  CFLAGS	+= -D__USE_INLINE__
  
  LDFLAGS	:= -nostartfiles
  LDFLAGS	+= -mcrt=newlib

  OBJDIR	:= $(OBJDIR)/USB3
  TARGET	:= usb3_printer.device

###########################################################################
# Set Target USB System
# -- unknown

else

  $(error Unknown Target USB System: $(USBTYPE). Please set USBTYPE to 'SIRION' or 'USB3')

endif

###########################################################################
# Set Compiler

ifeq ($(HOSTOS),AmigaOS)

LS			:= list
RM			:= delete
CC			:= gcc
STRIP		:= strip
MKDIR		:= makedir FORCE
COPY		:= copy
SDK			:= sdk:

# --

else ifeq ($(HOSTOS),Linux)

LS			:= ls -lort
RM			:= rm -rf
CC			:= ppc-amigaos-gcc
STRIP		:= ppc-amigaos-strip
MKDIR		:= mkdir -p
COPY		:= cp
SDK			:= /SDK/

endif

###########################################################################
# 

SRCS		+= src/Debug.c
SRCS		+= src/Device.c

SRCS		+= src/Process.c

SRCS		+= src/Unit_Alloc.c
SRCS		+= src/Unit_Free.c
SRCS		+= src/Unit_Startup.c

SRCS		+= src/Buffer_Flush.c
SRCS		+= src/Buffer_Write.c

SRCS		+= src/_cmd_0003_Write_Buffer.c
SRCS		+= src/_cmd_0003_Write_Direct.c
SRCS		+= src/_cmd_4000_DeviceQuery.c

###########################################################################

OBJS		:= $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
DEPS		:= $(patsubst %.c,$(OBJDIR)/%.d,$(SRCS))

all: $(BINDIR)/$(TARGET)

###############################################################################

$(BINDIR)/$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(OBJS) $(LIBS) -Wl,--cref,-M,-Map=usb3.map

###############################################################################

# Include dependency files
-include $(DEPS)

###############################################################################

clean:
	$(RM) $(OBJS) $(DEPS) $(BINDIR)/$(TARGET) usb3.map

###############################################################################

#revision:
#	bumprev -v $(VERSION) -r $(REVISION) -n $(TARGET) -b $(TARGET)

###############################################################################

install:
	copy $(BINDIR)/$(TARGET) $(INSTALLDIR)

###############################################################################

#installsdk:
#	$(MKDIR) $(SDK)local/common/include/devices
#	$(MKDIR) $(SDK)local/common/include/inline4
#	$(MKDIR) $(SDK)local/common/include/interfaces
#	$(MKDIR) $(SDK)local/common/include/proto
#	$(COPY) include/devices/usb3.h		$(SDK)local/common/include/devices
#	$(COPY) include/inline4/usb3.h		$(SDK)local/common/include/inline4
#	$(COPY) include/interfaces/usb3.h	$(SDK)local/common/include/interfaces
#	$(COPY) include/proto/usb3.h		$(SDK)local/common/include/proto

###############################################################################

#includes:
#	idltool -i -n -c usb3.xml

###############################################################################

strip:
	@$(LS)		$(BINDIR)/$(TARGET)
	@$(STRIP)	$(BINDIR)/$(TARGET)
	@$(LS)		$(BINDIR)/$(TARGET)

###############################################################################

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

###########################################################################
