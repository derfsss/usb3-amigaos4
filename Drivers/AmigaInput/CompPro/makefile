
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
#  USBTYPE - 'SIRIOS' 'USB3'		-- Target System
#

VERSION		:= 1
REVISION	:= 1

# Default
USBTYPE		?= SIRION

# Object Dir
OBJDIR		:= _obj

# Binary Dir
BINDIR		:= _bin

# Compiler Flags
CFLAGS		:= -O2
CFLAGS		+= -I.
CFLAGS		+= -I_inc
CFLAGS		+= -DDEBUG
CFLAGS		+= -gstabs
CFLAGS		+= -W
CFLAGS		+= -Wall
CFLAGS		+= -MMD
CFLAGS		+= -MP
CFLAGS		+= -mcrt=newlib

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

  SRCS		+= Sirion/_man_Init.c
  SRCS		+= Sirion/_man_Obtain.c
  SRCS		+= Sirion/_man_Release.c
  SRCS		+= Sirion/_man_Open.c
  SRCS		+= Sirion/_man_Close.c
  SRCS		+= Sirion/_man_Expunge.c
  SRCS		+= Sirion/_main_Obtain.c
  SRCS		+= Sirion/_main_Release.c
  SRCS		+= Sirion/_main_GetAttrs.c
  SRCS		+= Sirion/_main_RunFunction.c
  SRCS		+= Sirion/_main_RunInterface.c
  SRCS		+= Sirion/CompPro.c
  SRCS		+= Sirion/Library.c
  SRCS		+= Sirion/Resources_Setup.c
  SRCS		+= Sirion/Resources_Cleanup.c
  SRCS		+= Sirion/Driver_Function.c
  SRCS		+= Sirion/Driver_Init.c
  SRCS		+= Sirion/Driver_Free.c
  SRCS		+= Sirion/Driver_Main.c
  SRCS		+= Sirion/Driver_Handle_Stack.c
  SRCS		+= Sirion/Driver_Report_Get.c
  SRCS		+= Sirion/Driver_Report_Parse.c
  SRCS		+= Sirion/Driver_AI_Add.c
  SRCS		+= Sirion/Driver_AI_Rem.c

  CFLAGS	+= -ISirion
  CFLAGS	+= -D_SIRION_

  LDFLAGS	:= -nostartfiles
  LDFLAGS	+= -mcrt=newlib

#  LIBS		+=

  OBJDIR	:= $(OBJDIR)/Sirion
  TARGET	:= CompPro.usbfd
  INSTALLDIR := sys:devs/usb/fd

###########################################################################
# Set Target USB System
# -- USB3

else ifeq ($(USBTYPE),USB3)

  $(info Compiling for USB3)

  SRCS		+= USB3/CompPro.c

  CFLAGS	+= -IUSB3
  CFLAGS	+= -D_USB3_
#  CFLAGS	+= -DHAVE_APPLIB

  LDFLAGS	:= -nostartfiles
  LDFLAGS	+= -mcrt=newlib

 # SRCS		+= _aos4/rVNCd_ARexx.c

 # SRCS		+= _aos4/rVNCd_CxBroker.c
 # SRCS		+= _aos4/rVNCd_ZLib.c

 # TARGET	:= $(TARGET)_aos4
 # OBJDIR	:= _obj/_aos4
 # OSDIR		:= _aos4

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

SRCS		+= AmigaInput/_AmigaInput.c
SRCS		+= AmigaInput/_ai_CleanUp.c
SRCS		+= AmigaInput/_ai_HandleSettings.c
SRCS		+= AmigaInput/_ai_InitDriver.c
SRCS		+= AmigaInput/_ai_Obtain.c
SRCS		+= AmigaInput/_ai_Read_Kbd.c
SRCS		+= AmigaInput/_ai_Read.c
SRCS		+= AmigaInput/_ai_Release.c
SRCS		+= AmigaInput/_ai_SetCalibrateMode.c
SRCS		+= AmigaInput/_ai_SetEventMode.c
SRCS		+= AmigaInput/_ai_TestUnit.c

###########################################################################

OBJS		:= $(patsubst %.c,_obj/%.o,$(SRCS))
DEPS		:= $(patsubst %.c,_obj/%.d,$(SRCS))

all: _bin/$(TARGET)

###############################################################################

_bin/$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o _bin/$(TARGET) $(OBJS) $(LIBS) -Wl,--cref,-M,-Map=usb3.map

###############################################################################

# Include dependency files
-include $(DEPS)

###############################################################################

clean:
	$(RM) $(OBJS) $(DEPS) _bin/$(TARGET) usb3.map

###############################################################################

#revision:
#	bumprev -v $(VERSION) -r $(REVISION) -n $(TARGET) -b $(TARGET)

###############################################################################

install:
	copy _bin/$(TARGET) $(INSTALLDIR)

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
	@$(LS)		_bin/$(TARGET)
	@$(STRIP)	_bin/$(TARGET)
	@$(LS)		_bin/$(TARGET)

###############################################################################

_obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

###########################################################################
