# File author is Ítalo Lima Marconato Matias
#
# Created on September 01 of 2018, at 12:02 BRT
# Last edited on October 29 of 2018, at 16:19 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	PREFIX ?= i686-elf-
else
	UNSUPPORTED_ARCH := true
endif

ifeq ($(SUBARCH),)
	CDBOOT := boot/bootsect/build/cdboot-$(ARCH)
	BOOTMGR := boot/bootmgr/build/bootmgr-$(ARCH)
	KERNEL := kernel/build/chkrnl-$(ARCH)
else
	CDBOOT := boot/bootsect/build/cdboot-$(ARCH)_$(SUBARCH)
	BOOTMGR := boot/bootmgr/build/bootmgr-$(ARCH)_$(SUBARCH)
	KERNEL := kernel/build/chkrnl-$(ARCH)_$(SUBARCH)
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

.PHONY: boot kernel tools test

all: boot kernel tools test finish
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean: arch-clean
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot clean
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean
	$(NOECHO)VERBOSE=$(VERBOSE) make -C tools clean
	$(NOECHO)VERBOSE=$(VERBOSE) make -C test clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot clean-all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean-all
	$(NOECHO)VERBOSE=$(VERBOSE) make -C tools clean-all
	$(NOECHO)VERBOSE=$(VERBOSE) make -C test clean-all
	$(NOECHO)rm -rf build

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel remake
	$(NOECHO)VERBOSE=$(VERBOSE) make -C tools remake
	$(NOECHO)VERBOSE=$(VERBOSE) make -C test remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make finish

boot:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot all

kernel:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel all

tools:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C tools all

test:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C test all

ifeq ($(UNSUPPORTED_ARCH),true)
arch-clean:
	$(error Unsupported architecture $(ARCH))
finish:
	$(error Unsupported architecture $(ARCH))
else ifeq ($(ARCH),x86)
arch-clean:
ifeq ($(SUBARCH),)
	$(NOECHO)rm build/chicago-$(ARCH).iso
else
	$(NOECHO)rm Generating build/chicago-$(ARCH)_$(SUBARCH).iso
endif
finish:
ifeq ($(SUBARCH),)
	$(NOECHO)echo Generating build/chicago-$(ARCH).iso
else
	$(NOECHO)echo Generating build/chicago-$(ARCH)_$(SUBARCH).iso
endif
	$(NOECHO)if [ ! -d build ]; then mkdir -p build; fi
	$(NOECHO)if [ -d build/iso ]; then rm -rf build/iso; fi
	$(NOECHO)mkdir -p build/iso/{Boot,MountPoint}
	$(NOECHO)cp $(CDBOOT) build/iso/Boot/bootsect.bin
	$(NOECHO)cp $(BOOTMGR) build/iso/Boot/bootmgr.bin
	$(NOECHO)cp $(KERNEL) build/iso/Boot/chkrnl.elf
	$(NOECHO)cp test/build/test.bliss build/iso/test.bliss
	$(NOECHO)echo '"Boot from CHicago Install CD"=BootDevice,chicago' >> build/iso/Boot/bootmgr.conf
ifeq ($(SUBARCH),)
	$(NOECHO)xorriso -as mkisofs -R -c Boot/boot.cat -b Boot/bootsect.bin -U -no-emul-boot -o build/chicago-$(ARCH).iso build/iso 2>/dev/null
else
	$(NOECHO)xorriso -as mkisofs -R -c Boot/boot.cat -b Boot/bootsect.bin -U -no-emul-boot -o build/chicago-$(ARCH)_$(SUBARCH).iso build/iso 2>/dev/null
endif
	$(NOECHO)rm -rf build/iso
else
arch-clean:
finish:
endif
