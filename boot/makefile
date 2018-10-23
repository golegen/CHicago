# File author is Ítalo Lima Marconato Matias
#
# Created on October 22 of 2018, at 13:32 BRT
# Last edited on October 23 of 2018, at 14:49 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	PREFIX ?= i686-elf-
	
	ifneq ($(SUBARCH),)
		UNSUPPORTED_ARCH := true
	endif
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

.PHONY: bootsect bootmgr

all: bootsect bootmgr
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect clean
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect clean-all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr clean-all

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr remake

bootsect:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect all

bootmgr:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) PREFIX=$(PREFIX) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr all
