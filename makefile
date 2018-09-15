# File author is Ítalo Lima Marconato Matias
#
# Created on September 01 of 2018, at 12:02 BRT
# Last edited on September 15 of 2018, at 11:56 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	PREFIX ?= i686-chicago-
else ifeq ($(ARCH),arm)
	PREFIX ?= arm-none-eabi-
	SUBARCH ?= vexpress
else
	UNSUPPORTED_ARCH := true
endif

ifeq ($(SUBARCH),)
	KERNEL := kernel/build/chkrnl-$(ARCH)
else
	KERNEL := kernel/build/chkrnl-$(ARCH)_$(SUBARCH)
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

.PHONY: $(KERNEL)

all: $(KERNEL) finish
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean-all

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make finish

$(KERNEL):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel all

ifeq ($(UNSUPPORTED_ARCH),true)
finish:
	$(error Unsupported architecture $(ARCH))
else ifeq ($(ARCH),x86)
finish:
ifeq ($(SUBARCH),)
	$(NOECHO)echo Generating build/chicago-$(ARCH).iso
else
	$(NOECHO)echo Generating build/chicago-$(ARCH)_$(SUBARCH).iso
endif
	$(NOECHO)if [ ! -d build ]; then mkdir -p build; fi
	$(NOECHO)if [ -d build/iso ]; then rm -rf build/iso; fi
	$(NOECHO)mkdir -p build/iso/boot/grub
	$(NOECHO)cp $(KERNEL) build/iso/boot/chkrnl.che
	$(NOECHO)echo 'set timeout=30' > build/iso/boot/grub/grub.cfg
	$(NOECHO)echo 'set default=0' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo 'menuentry "Boot from CHicago Install CD" {' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '    multiboot /boot/chkrnl.che' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '    boot' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '}' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo 'menuentry "Boot from Hard Disk" {' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '    chainloader (hd0)+1' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '}' >> build/iso/boot/grub/grub.cfg
	$(NOECHO)echo '' >> build/iso/boot/grub/grub.cfg
ifeq ($(SUBARCH),)
	$(NOECHO)grub-mkrescue --directory=/usr/lib/grub/i386-pc --output=build/chicago-$(ARCH).iso build/iso -c boot/boot.cat 2>/dev/null
else
	$(NOECHO)grub-mkrescue --directory=/usr/lib/grub/i386-pc --output=build/chicago-$(ARCH)_$(SUBARCH).iso build/iso -c boot/boot.cat 2>/dev/null
endif
	$(NOECHO)rm -rf build/iso
else
finish:
endif
