# File author is Ítalo Lima Marconato Matias
#
# Created on May 11 of 2018, at 13:14 BRT
# Last edited on September 16 of 2018, at 12:37 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	PREFIX ?= i686-chicago-
	
	ifneq ($(SUBARCH),)
		UNSUPPORTED_ARCH := true
	endif
	
	ARCH_OBJECTS := start.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/debug.c.o io/ide.c.o io/mbdisp.c.o
	ARCH_OBJECTS += sys/gdt.c.o sys/idt.c.o sys/pit.c.o sys/process.c.o
	ARCH_OBJECTS += sys/sc.c.o
	ARCH_OBJECTS += mm/pmm.c.o mm/vmm.c.o
	
	LINKER_SCRIPT := link.ld
else ifeq ($(ARCH),arm)
	PREFIX ?= arm-none-eabi-
	SUBARCH ?= vexpress
	
	ARCH_OBJECTS := start.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/debug.c.o
	
	ifeq ($(SUBARCH),vexpress)
		ARCH_AFLAGS	:= -march=armv7-a -mcpu=cortex-a15 -mfloat-abi=hard -mfpu=neon
		ARCH_CFLAGS := -march=armv7-a -mtune=cortex-a15 -mfloat-abi=hard -mfpu=neon
	else
		UNSUPPORTED_ARCH := true
	endif
	
	LINKER_SCRIPT := subarch/$(SUBARCH)/link.ld
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := main.c.o
OBJECTS += ds/list.c.o
OBJECTS += io/device.c.o io/display.c.o io/file.c.o
OBJECTS += io/dev/framebuffer.c.o io/dev/null.c.o io/dev/zero.c.o
OBJECTS += io/fs/devfs.c.o io/fs/iso9660.c.o
OBJECTS += mm/alloc.c.o mm/heap.c.o mm/pmm.c.o mm/virt.c.o
OBJECTS += sys/process.c.o sys/sc.c.o sys/string.c.o

ARCH_OBJECTS := $(addprefix build/arch/$(ARCH)/,$(ARCH_OBJECTS))
OBJECTS := $(addprefix build/,$(OBJECTS))
LINKER_SCRIPT := arch/$(ARCH)/$(LINKER_SCRIPT)

ifeq ($(SUBARCH),)
	KERNEL := build/chkrnl-$(ARCH)
else
	KERNEL := build/chkrnl-$(ARCH)_$(SUBARCH)
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(KERNEL)

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -f $(ARCH_OBJECTS) $(OBJECTS) $(CRTI) $(CRTN) $(KERNEL)

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

remake: clean all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

$(KERNEL): $(ARCH_OBJECTS) $(OBJECTS) $(LINKER_SCRIPT)
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
	$(NOECHO)$(PREFIX)gcc -DARCH=\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough $(ARCH_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(PREFIX)gcc -DARCH=\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -I arch/$(ARCH)/subarch/$(SUBARCH)/include -ffreestanding -O3 -Wall -Wextra -Wno-implicit-fallthrough $(ARCH_CFLAGS) -c $< -o $@
endif
