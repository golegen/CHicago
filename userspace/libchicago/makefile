# File author is Ítalo Lima Marconato Matias
# 
# Created on November 15 of 2018, at 17:39 BRT
# Last edited on November 16 of 2018, at 09:22 BRT

SYSROOT_DIR ?=
SYS_LIB_DIR ?= System/Libraries
DEV_LIB_DIR ?= Development/Libraries
DEV_INC_DIR ?= Development/Headers
ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
	
	ARCH_OBJECTS := io/file.c.o
	ARCH_OBJECTS += mm/alloc.c.o mm/virt.c.o
	ARCH_OBJECTS += sys/process.c.o sys/version.c.o
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := sys/string.c.o

ARCH_OBJECTS := $(addprefix build/arch/$(ARCH)/,$(ARCH_OBJECTS))
OBJECTS := $(addprefix build/,$(OBJECTS))
DEV_OUTPUT := build/libchicago.so
SYS_OUTPUT := build/chicago.chl

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(DEV_OUTPUT) $(SYS_OUTPUT)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -f $(ARCH_OBJECTS) $(OBJECTS) $(DEV_OUTPUT) $(SYS_OUTPUT)

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

remake: clean all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

install: all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Installing headers
	$(NOECHO)cp -RT include $(SYSROOT_DIR)/$(DEV_INC_DIR)
	$(NOECHO)echo Installing libchicago.so
	$(NOECHO)cp $(DEV_OUTPUT) $(SYSROOT_DIR)/$(DEV_LIB_DIR)/
	$(NOECHO)echo Installing chicago.chl
	$(NOECHO)cp $(SYS_OUTPUT) $(SYSROOT_DIR)/$(SYS_LIB_DIR)/

$(SYS_OUTPUT): $(DEV_OUTPUT)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)objconv -l -o $@ $(DEV_OUTPUT)

$(DEV_OUTPUT): $(ARCH_OBJECTS) $(OBJECTS)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -nostdlib -shared -o $@ $(ARCH_OBJECTS) $(OBJECTS) $(ARCH_LDFLAGS) -lgcc

build/%.s.o: %.s
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-as $(ARCH_AFLAGS) $< -o $@

build/%.c.o: %.c
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -std=c11 -Iinclude -O3 -fno-builtin -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -c $< -o $@
