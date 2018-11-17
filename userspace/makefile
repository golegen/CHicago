# File author is Ítalo Lima Marconato Matias
#
# Created on November 15 of 2018, at 17:26 BRT
# Last edited on November 17 of 2018, at 13:14 BRT

SYSROOT_DIR ?=
SYS_LIB_DIR ?= System/Libraries
SYS_PROG_DIR ?= System/Programs
DEV_LIB_DIR ?= Development/Libraries
DEV_PROG_DIR ?= Development/Programs
DEV_INC_DIR ?= Development/Headers
ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

.PHONY: libchicago sesmgr

all: libchicago sesmgr
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_LIB_DIR=$(SYS_LIB_DIR) DEV_LIB_DIR=$(DEV_LIB_DIR) DEV_INC_DIR=$(DEV_INC_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C libchicago clean
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_PROG_DIR=$(SYS_PROG_DIR) DEV_PROG_DIR=$(DEV_PROG_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C sesmgr clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_LIB_DIR=$(SYS_LIB_DIR) DEV_LIB_DIR=$(DEV_LIB_DIR) DEV_INC_DIR=$(DEV_INC_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C libchicago clean-all
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_PROG_DIR=$(SYS_PROG_DIR) DEV_PROG_DIR=$(DEV_PROG_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C sesmgr clean-all

remake:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_LIB_DIR=$(SYS_LIB_DIR) DEV_LIB_DIR=$(DEV_LIB_DIR) DEV_INC_DIR=$(DEV_INC_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C libchicago remake
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_LIB_DIR=$(SYS_LIB_DIR) DEV_LIB_DIR=$(DEV_LIB_DIR) DEV_INC_DIR=$(DEV_INC_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C libchicago install
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_PROG_DIR=$(SYS_PROG_DIR) DEV_PROG_DIR=$(DEV_PROG_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C sesmgr remake
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_PROG_DIR=$(SYS_PROG_DIR) DEV_PROG_DIR=$(DEV_PROG_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C sesmgr install

libchicago:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_LIB_DIR=$(SYS_LIB_DIR) DEV_LIB_DIR=$(DEV_LIB_DIR) DEV_INC_DIR=$(DEV_INC_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C libchicago all
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_LIB_DIR=$(SYS_LIB_DIR) DEV_LIB_DIR=$(DEV_LIB_DIR) DEV_INC_DIR=$(DEV_INC_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C libchicago install

sesmgr:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_PROG_DIR=$(SYS_PROG_DIR) DEV_PROG_DIR=$(DEV_PROG_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C sesmgr all
	$(NOECHO)SYSROOT_DIR=$(SYSROOT_DIR) SYS_PROG_DIR=$(SYS_PROG_DIR) DEV_PROG_DIR=$(DEV_PROG_DIR) ARCH=$(ARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C sesmgr install
