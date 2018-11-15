# File author is Ítalo Lima Marconato Matias
#
# Created on October 22 of 2018, at 13:39 BRT
# Last edited on November 13 of 2018, at 16:32 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
	
	ifneq ($(SUBARCH),)
		UNSUPPORTED_ARCH := true
	endif
	
	LINKER_SCRIPT := link.ld
else
	UNSUPPORTED_ARCH := true
endif

ifeq ($(SUBARCH),)
	OBJECTS := cdboot-$(ARCH)
else
	OBJECT := cdboot-$(ARCH)_$(SUBARCH)
endif

OBJECTS := $(addprefix build/,$(OBJECTS))
LINKER_SCRIPT := arch/$(ARCH)/$(LINKER_SCRIPT)

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(OBJECTS)

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -f $(OBJECTS)

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

remake: clean all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

ifeq ($(SUBARCH),)
build/%-$(ARCH): arch/$(ARCH)/%.s
else
build/%-$(ARCH)_$(SUBARCH): arch/$(ARCH)/$(SUBARCH)/%.s
endif
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-as $(ARCH_AFLAGS) $< -o $@.o
	$(NOECHO)$(TARGET)-ld -T$(LINKER_SCRIPT) -o $@ $@.o
	$(NOECHO)rm $@.o
