# File author is Ítalo Lima Marconato Matias
#
# Created on September 01 of 2018, at 12:02 BRT
# Last edited on January 27 of 2019, at 14:28 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false
SYS_LANG ?= en

ifeq ($(ARCH),x86)
	SUBARCH ?= 32
	TARGET ?= i686-elf
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(SYS_LANG),en)
ifneq ($(SYS_LANG),br)
	UNSUPPORTED_LANG := true
endif
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: toolchain
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
	$(NOECHO)BUILD_CORES=$(BUILD_CORES) ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) make -C toolchain all
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYS_LANG=$(SYS_LANG) make finish

clean: arch-clean
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) clean
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) clean-all
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean-all
	$(NOECHO)rm -rf build

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) remake
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYS_LANG=$(SYS_LANG) make finish

include arch/$(ARCH)/arch.mk
