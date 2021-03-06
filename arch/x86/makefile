# File author is Ítalo Lima Marconato Matias
#
# Created on December 19 of 2018, at 16:12 BRT
# Last edited on March 29 of 2019, at 18:48 BRT

SUBARCH ?= 32
DEBUG ?= false
VERBOSE ?= false

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/x86/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(SUBARCH),64)
	TARGET := x86_64-w64-mingw32
else
	TARGET := i686-w64-mingw32
endif

OBJECTS := jump.s.o
OBJECTS += arch.c.o data.c.o main.c.o start.c.o
OBJECTS += ds/list.c.o
OBJECTS += io/console.c.o io/device.c.o io/file.c.o io/keyboard.c.o
OBJECTS += io/fs/devfs.c.o
OBJECTS += io/fs/iso9660.c.o
OBJECTS += mm/alloc.c.o
OBJECTS += sys/chexec.c.o sys/config.c.o sys/string.c.o sys/menu.c.o

OBJECTS := $(addprefix build/$(SUBARCH)/,$(OBJECTS))
BOOTMGR := build/bootmgr$(SUBARCH)

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(BOOTMGR)

clean:
	$(NOECHO)rm -f $(SUBARCH_OBJECTS) $(OBJECTS) $(BOOTMGR)

clean-all:
	$(NOECHO)rm -rf build

remake: clean all

$(BOOTMGR): $(SUBARCH_OBJECTS) $(OBJECTS)
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(SUBARCH),64)
	$(NOECHO)$(TARGET)-gcc -nostdlib -shared -e LoaderEntry -Wl,-dll -Wl,--subsystem,10 $(SUBARCH_OBJECTS) $(OBJECTS) -o $(BOOTMGR).efi
else
	$(NOECHO)$(TARGET)-gcc -nostdlib -shared -e _LoaderEntry -Wl,-dll -Wl,--subsystem,10 $(SUBARCH_OBJECTS) $(OBJECTS) -o $(BOOTMGR).efi
endif
	$(NOECHO)cp $(BOOTMGR).efi $(BOOTMGR)
	$(NOECHO)rm $(BOOTMGR).efi

build/$(SUBARCH)/%.s.o: %.s
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(SUBARCH),64)
	$(NOECHO)$(TARGET)-as --defsym ARCH_64=1 $< -o $@
else
	$(NOECHO)$(TARGET)-as $< -o $@
endif

build/$(SUBARCH)/%.c.o: %.c
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(SUBARCH),64)
ifeq ($(DEBUG),true)
	$(NOECHO)$(TARGET)-gcc -DARCH_64 -DDEBUG -g -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -DARCH_64 -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
endif
else
ifeq ($(DEBUG),true)
	$(NOECHO)$(TARGET)-gcc -DDEBUG -g -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -I/usr/include/efi -I/usr/include/efi/ia32 -I/usr/include/efi/protocol -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -std=c11 -Iinclude -Isubarch/$(SUBARCH)/include -I/usr/include/efi -I/usr/include/efi/ia32 -I/usr/include/efi/protocol -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough -c $< -o $@
endif
endif
