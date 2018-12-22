# File author is Ítalo Lima Marconato Matias
#
# Created on December 19 of 2018, at 16:12 BRT
# Last edited on December 21 of 2018, at 23:59 BRT

SUBARCH ?= pc
DEBUG ?= false
VERBOSE ?= false
TARGET ?= i686-elf

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/x86/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(SUBARCH),efi)
	TARGET := i686-w64-mingw32
	
	SUBARCH_OBJECTS := data.c.o start.c.o subarch.c.o
	SUBARCH_OBJECTS += io/console.c.o
	SUBARCH_OBJECTS += mm/alloc.c.o
else
	SUBARCH_OBJECTS := start.s.o int86.s.o
	SUBARCH_OBJECTS += subarch.c.o
	SUBARCH_OBJECTS += io/console.c.o io/ide.c.o
	SUBARCH_OBJECTS += mm/alloc.c.o mm/heap.c.o
	SUBARCH_OBJECTS += sys/idt.c.o
endif

OBJECTS := jump.s.o
OBJECTS += main.c.o
OBJECTS += ds/list.c.o
OBJECTS += io/console.c.o io/device.c.o io/file.c.o io/keyboard.c.o
OBJECTS += io/fs/devfs.c.o
OBJECTS += io/fs/iso9660.c.o
OBJECTS += mm/alloc.c.o
OBJECTS += sys/config.c.o sys/string.c.o sys/menu.c.o

SUBARCH_OBJECTS := $(addprefix build/$(SUBARCH)/subarch/$(SUBARCH)/,$(SUBARCH_OBJECTS))
OBJECTS := $(addprefix build/$(SUBARCH)/,$(OBJECTS))
BOOTMGR := build/$(SUBARCH)/bootmgr

ifneq ($(VERBOSE),true)
NOECHO := @
endif

ifneq ($(SUBARCH),efi)
all: subarch $(BOOTMGR)
else
all: $(BOOTMGR)
endif

ifneq ($(SUBARCH),efi)
clean: subarch-clean
else
clean:
endif
	$(NOECHO)rm -f $(SUBARCH_OBJECTS) $(OBJECTS) $(BOOTMGR)

clean-all:
	$(NOECHO)rm -rf build

remake: clean all

ifeq ($(SUBARCH),efi)
$(BOOTMGR): $(SUBARCH_OBJECTS) $(OBJECTS)
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -nostdlib -shared -e _LoaderEntry -Wl,-dll -Wl,--subsystem,10 $(SUBARCH_OBJECTS) $(OBJECTS) -o $(BOOTMGR).efi
	$(NOECHO)cp $(BOOTMGR).efi $(BOOTMGR)
	$(NOECHO)rm $(BOOTMGR).efi
else
$(BOOTMGR): $(SUBARCH_OBJECTS) $(OBJECTS) subarch/$(SUBARCH)/link.ld
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -Tsubarch/$(SUBARCH)/link.ld -ffreestanding -nostdlib -o $@ $(SUBARCH_OBJECTS) $(OBJECTS) -lgcc
endif

build/$(SUBARCH)/%.s.o: %.s
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(SUBARCH),efi)
	$(NOECHO)$(TARGET)-as --defsym SUBARCH_EFI=1 $< -o $@
else
	$(NOECHO)$(TARGET)-as $< -o $@
endif

build/$(SUBARCH)/%.c.o: %.c
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(SUBARCH),efi)
ifeq ($(DEBUG),true)
	$(NOECHO)$(TARGET)-gcc -DDEBUG -g -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -I/usr/include/efi -I/usr/include/efi/ia32 -I/usr/include/efi/protocol -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -I/usr/include/efi -I/usr/include/efi/ia32 -I/usr/include/efi/protocol -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
endif
else
ifeq ($(DEBUG),true)
	$(NOECHO)$(TARGET)-gcc -DDEBUG -g -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
endif
endif

ifneq ($(SUBARCH),efi)
include subarch/$(SUBARCH)/subarch.mk
endif
