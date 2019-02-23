# File author is Ítalo Lima Marconato Matias
#
# Created on February 23 of 2019, at 14:32 BRT
# Last edited on February 23 of 2019, at 15:29 BRT

VERBOSE ?= false
DEBUG ?= false

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../../toolchain/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

OBJECTS := main.asm.o
OBJECTS += io/file.asm.o
OBJECTS += mm/alloc.asm.o mm/virt.asm.o
OBJECTS += sys/ipc.asm.o sys/process.asm.o sys/version.asm.o

OBJECTS := $(addprefix build/,$(OBJECTS))
OUTPUT := build/sesmgr.che

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(OUTPUT)

clean:
	$(NOECHO)rm -f $(OBJECTS) $(OUTPUT)

clean-all:
	$(NOECHO)rm -rf build

remake: clean all

$(OUTPUT): $(OBJECTS)
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)chlink -o $@ $(OBJECTS)

build/%.asm.o: %.asm
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)chasm $< -o $@
