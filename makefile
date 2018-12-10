# File author is Ítalo Lima Marconato Matias
#
# Created on September 01 of 2018, at 12:02 BRT
# Last edited on December 10 of 2018, at 16:39 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
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

.PHONY: boot kernel

all: toolchain/$(ARCH) boot kernel finish
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean: arch-clean sysroot-recreate
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot clean
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean

clean-all: sysroot-recreate
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot clean-all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean-all
	$(NOECHO)rm -rf build

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make finish

toolchain/$(ARCH):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)BUILD_CORES=$(BUILD_CORES) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) make -C toolchain all clean

boot:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C boot all

kernel:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel all

sysroot-recreate:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf toolchain/$(ARCH)/sysroot/*
	$(NOECHO)mkdir -p toolchain/$(ARCH)/sysroot/{Development,System}
	$(NOECHO)mkdir -p toolchain/$(ARCH)/sysroot/Development/{Headers,Libraries,Programs,Sources}
	$(NOECHO)mkdir -p toolchain/$(ARCH)/sysroot/System/{Configurations,Libraries,Programs}

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
	$(NOECHO)mkdir -p build/iso/Boot
	$(NOECHO)mkdir -p build/iso/MountPoint
	$(NOECHO)echo -e 'Hello, World! This is a test file!\r' > build/iso/TestEn.txt
	$(NOECHO)echo -e 'Olá Mundo! Este é um arquivo de teste!\r' > build/iso/TestBr.txt
	$(NOECHO)cp $(CDBOOT) build/iso/Boot/bootsect.bin
	$(NOECHO)cp $(BOOTMGR) build/iso/Boot/bootmgr.bin
	$(NOECHO)cp $(KERNEL) build/iso/Boot/chkrnl.elf
	$(NOECHO)cp -RT toolchain/$(ARCH)/sysroot/ build/iso/
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
