# File author is Ítalo Lima Marconato Matias
# 
# Created on November 16 of 2018, at 22:44 BRT
# Last edited on November 16 of 2018, at 22:45 BRT

SYSROOT_DIR ?=
SYS_PROG_DIR ?= System/Programs
DEV_PROG_DIR ?= Development/Programs
ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := main.c.o

OBJECTS := $(addprefix build/,$(OBJECTS))
DEV_OUTPUT := build/sesmgr
SYS_OUTPUT := build/sesmgr.che

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
	$(NOECHO)rm -f $(OBJECTS) $(DEV_OUTPUT) $(SYS_OUTPUT)

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
	$(NOECHO)echo Installing sesmgr
	$(NOECHO)cp $(DEV_OUTPUT) $(SYSROOT_DIR)/$(DEV_PROG_DIR)/
	$(NOECHO)echo Installing sesmgr.che
	$(NOECHO)cp $(SYS_OUTPUT) $(SYSROOT_DIR)/$(SYS_PROG_DIR)/

$(SYS_OUTPUT): $(DEV_OUTPUT)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)objconv -e -o $@ $(DEV_OUTPUT)

$(DEV_OUTPUT): $(OBJECTS)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -nostdlib -o $@ $(OBJECTS) $(ARCH_LDFLAGS) -lchicago -lgcc

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
