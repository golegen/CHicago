# File author is Ítalo Lima Marconato Matias
#
# Created on October 22 of 2018, at 13:32 BRT
# Last edited on November 12 of 2018, at 13:07 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
	
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
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect clean
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect clean-all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr clean-all

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr remake

bootsect:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootsect all

bootmgr:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C bootmgr all
