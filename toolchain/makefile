# File author is Ítalo Lima Marconato Matias
#
# Created on November 11 of 2018, at 20:35 BRT
# Last edited on December 19 of 2018, at 18:48 BRT

ARCH ?= x86
VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PATH := $(ROOT_DIR)/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	MAIN_TARGET := i686-elf
	SECOND_TARGET := i686-w64-mingw32
	SECOND_TARGET_NO_LIBGCC := true
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

ifeq ($(SECOND_TARGET),)
all: $(ARCH)/$(MAIN_TARGET)
else
all: $(ARCH)/$(MAIN_TARGET) $(ARCH)/$(SECOND_TARGET)
endif
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf $(ARCH) build

remake: clean-all all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

$(ARCH)/$(MAIN_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)/$(MAIN_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ -d build ]; then rm -rf build; fi
	$(NOECHO)mkdir build
	$(NOECHO)echo Downloading Binutils source code
	$(NOECHO)cd build && wget -q https://ftp.gnu.org/gnu/binutils/binutils-2.31.1.tar.gz
	$(NOECHO)echo Downloading GCC source code
	$(NOECHO)cd build && wget -q https://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz
	$(NOECHO)echo Extracting Binutils source code
	$(NOECHO)cd build && tar xpf binutils-2.31.1.tar.gz
	$(NOECHO)echo Extracting GCC source code
	$(NOECHO)cd build && tar xpf gcc-8.2.0.tar.gz
	$(NOECHO)echo Compiling Binutils \($(MAIN_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.31.1/build && cd build/binutils-2.31.1/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --with-sysroot --disable-nls --disable-werror 1>/dev/null 2>1 && make -j$(BUILD_CORES) 1>/dev/null 2>1 && make install -j$(BUILD_CORES) 1>/dev/null 2>1
	$(NOECHO)echo Compiling GCC \($(MAIN_TARGET)\)
ifneq ($(MAIN_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
else
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc 1>/dev/null 2>1
endif

ifneq ($(SECOND_TARGET),)
$(ARCH)/$(SECOND_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)/$(SECOND_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ ! -d build ]; then exit 1; fi
	$(NOECHO)rm -rf build/binutils-2.31.1/build build/gcc-8.2.0/build
	$(NOECHO)echo Compiling Binutils \($(SECOND_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.31.1/build && cd build/binutils-2.31.1/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --with-sysroot --disable-nls --disable-werror 1>/dev/null 2>1 && make -j$(BUILD_CORES) 1>/dev/null 2>1 && make install -j$(BUILD_CORES) 1>/dev/null 2>1
	$(NOECHO)echo Compiling GCC \($(SECOND_TARGET)\)
ifneq ($(SECOND_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
else
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc 1>/dev/null 2>1
endif
endif
