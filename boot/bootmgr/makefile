# File author is Ítalo Lima Marconato Matias
#
# Created on October 22 of 2018, at 15:35 BRT
# Last edited on October 25 of 2018, at 15:18 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	PREFIX ?= i686-elf-
	
	ifneq ($(SUBARCH),)
		UNSUPPORTED_ARCH := true
	endif
	
	ARCH_OBJECTS := start.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/console.c.o io/ide.c.o io/keyboard.c.o
	ARCH_OBJECTS += sys/idt.c.o
	
	LINKER_SCRIPT := link.ld
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := main.c.o
OBJECTS += ds/list.c.o
OBJECTS += io/console.c.o io/device.c.o io/file.c.o io/fs/devfs.c.o
OBJECTS += io/fs/iso9660.c.o
OBJECTS += mm/alloc.c.o mm/heap.c.o
OBJECTS += sys/config.c.o sys/menu.c.o sys/string.c.o

ARCH_OBJECTS := $(addprefix build/arch/$(ARCH)/,$(ARCH_OBJECTS))
OBJECTS := $(addprefix build/,$(OBJECTS))
LINKER_SCRIPT := arch/$(ARCH)/$(LINKER_SCRIPT)

ifeq ($(SUBARCH),)
	BOOTMGR := build/bootmgr-$(ARCH)
else
	BOOTMGR := build/bootmgr-$(ARCH)_$(SUBARCH)
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(BOOTMGR)

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -f $(ARCH_OBJECTS) $(OBJECTS) $(BOOTMGR)

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

remake: clean all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

$(BOOTMGR): $(ARCH_OBJECTS) $(OBJECTS) $(LINKER_SCRIPT)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(PREFIX)gcc -T$(LINKER_SCRIPT) -ffreestanding -nostdlib -o $@ $(ARCH_OBJECTS) $(OBJECTS) $(ARCH_LDFLAGS) -lgcc

build/%.s.o: %.s
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(PREFIX)as $(ARCH_AFLAGS) $< -o $@

build/%.c.o: %.c
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(SUBARCH),)
ifeq ($(DEBUG),yes)
	$(NOECHO)$(PREFIX)gcc -DARCH=\"$(ARCH)\" -DDEBUG -g -std=c11 -Iinclude -Iarch/$(ARCH)/include -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(PREFIX)gcc -DARCH=\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
endif
else
ifeq ($(DEBUG),yes)
	$(NOECHO)$(PREFIX)gcc -DARCH=\"$(ARCH)\" -DDEBUG -g -std=c11 -Iinclude -Iarch/$(ARCH)/include -I arch/$(ARCH)/subarch/$(SUBARCH)/include -ffreestanding -O0 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(PREFIX)gcc -DARCH=\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -I arch/$(ARCH)/subarch/$(SUBARCH)/include -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
endif
endif
