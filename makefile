# File author is Ítalo Lima Marconato Matias
#
# Created on September 01 of 2018, at 12:02 BRT
# Last edited on December 20 of 2018, at 11:11 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	TARGET ?= i686-elf
	SUBARCH ?= pc

	ifneq ($(SUBARCH),pc)
	ifneq ($(SUBARCH),efi)
		UNSUPPORTED_ARCH := true
	endif
	endif
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: toolchain/$(ARCH)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make finish

clean: arch-clean
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) clean-all
	$(NOECHO)rm -rf build

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make finish

toolchain/$(ARCH):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)BUILD_CORES=$(BUILD_CORES) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) make -C toolchain all clean

include arch/x86/arch.mk
