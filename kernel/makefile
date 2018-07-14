# File author is Ítalo Lima Marconato Matias
#
# Created on May 11 of 2018, at 13:14 BRT
# Last edited on July 14 of 2018, at 00:28 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	PREFIX ?= i686-elf-
	
	ARCH_OBJECTS := start.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/debug.c.o
	ARCH_OBJECTS += system/gdt.c.o system/idt.c.o
	ARCH_OBJECTS += mem/pmm.c.o mem/vmm.c.o
	
	LINKER_SCRIPT := link.ld
else ifeq ($(ARCH),x86-64)
	PREFIX ?= x86_64-elf-
	
	ARCH_OBJECTS := start32.s.o start64.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/debug.c.o
	ARCH_OBJECTS += system/gdt.c.o system/idt.c.o
	ARCH_OBJECTS += mem/pmm.c.o
	
	LINKER_SCRIPT := link.ld
	
	ARCH_CFLAGS := -DARCH_64 -DMM_PAGE_SIZE=0x200000 -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wno-int-to-pointer-cast
	ARCH_LDFLAGS := -mcmodel=large -mno-red-zone -n
else ifeq ($(ARCH),raspi)
	PREFIX ?= arm-none-eabi-
	
	ARCH_OBJECTS := start.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/debug.c.o
	
	LINKER_SCRIPT := link.ld
else ifeq ($(ARCH),raspi2)
	PREFIX ?= arm-none-eabi-
	
	ARCH_OBJECTS := start.s.o
	ARCH_OBJECTS += arch.c.o
	ARCH_OBJECTS += io/debug.c.o
	
	LINKER_SCRIPT := link.ld
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := main.c.o
OBJECTS += mem/alloc.c.o mem/heap.c.o mem/pmm.c.o

ARCH_OBJECTS := $(addprefix build/arch/$(ARCH)/,$(ARCH_OBJECTS))
OBJECTS := $(addprefix build/,$(OBJECTS))
LINKER_SCRIPT := arch/$(ARCH)/$(LINKER_SCRIPT)
KERNEL := build/chkrnl-$(ARCH)

ifneq ($(VERBOSE),true)
NOECHO := @
endif

ifneq ($(ARCH),raspi)
ifneq ($(ARCH),raspi2)
all: build/chicago-$(ARCH).iso
else
all: $(KERNEL)
endif
else
all: $(KERNEL)
endif
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

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

ifneq ($(ARCH),raspi)
ifneq ($(ARCH),raspi2)
build/chicago-$(ARCH).iso: build/chkrnl-$(ARCH)
	$(NOECHO)echo Generating $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)if [ -d $(dir $@)/iso ]; then rm -rf $(dir $@)/iso; fi
	$(NOECHO)mkdir -p $(dir $@)/iso
	$(NOECHO)mkdir -p $(dir $@)/iso/boot
	$(NOECHO)mkdir -p $(dir $@)/iso/boot/grub
	$(NOECHO)cp build/chkrnl-$(ARCH) $(dir $@)/iso/boot/chkrnl
	$(NOECHO)echo 'set timeout=30' > $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo 'set default=0' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo '' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo 'menuentry "Boot from Hard Disk" {' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo '    chainloader (hd0)+1' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo '}' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo '' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo 'menuentry "CHicago Operating System" {' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo '    multiboot /boot/chkrnl' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo '    boot' >> $(dir $@)/iso/boot/grub/grub.cfg
	$(NOECHO)echo '}' >> $(dir $@)/iso/boot/grub/grub.cfg
ifdef ($(GRUB_DIR))
	$(NOECHO)grub-mkrescue -d $(GRUB_DIR) --output=$(dir $@)/chicago-$(ARCH).iso $(dir $@)/iso 2>/dev/null
else
	$(NOECHO)grub-mkrescue --output=$(dir $@)/chicago-$(ARCH).iso $(dir $@)/iso 2>/dev/null
endif
	$(NOECHO)rm -rf $(dir $@)/iso
endif
endif

$(KERNEL): $(ARCH_OBJECTS) $(OBJECTS) $(LINKER_SCRIPT)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)TZ=America/Sao_Paulo $(PREFIX)gcc -T$(LINKER_SCRIPT) -ffreestanding -nostdlib -o $@ $(ARCH_OBJECTS) $(OBJECTS) $(ARCH_LDFLAGS) -lgcc

build/%.s.o: %.s
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(PREFIX)as $< -o $@

build/%.c.o: %.c
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(PREFIX)gcc -DARCH=\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -ffreestanding -O2 -Wall -Wextra -Wno-implicit-fallthrough $(ARCH_CFLAGS) -c $< -o $@
