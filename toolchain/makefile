# File author is Ítalo Lima Marconato Matias
#
# Created on November 11 of 2018, at 20:35 BRT
# Last edited on January 23 of 2018, at 13:50 BRT

ARCH ?= x86
VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PATH := $(ROOT_DIR)/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	SUBARCH ?= 32
ifeq ($(SUBARCH),32)
	MAIN_TARGET := i686-elf
	SECOND_TARGET := i686-w64-mingw32
	SECOND_TARGET_NO_LIBGCC := true
else
	MAIN_TARGET := x86_64-elf
	MAIN_TARGET_MCMODEL_LARGE := true
	SECOND_TARGET := x86_64-w64-mingw32
	SECOND_TARGET_NO_LIBGCC := true
endif
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

TARGETS := $(ARCH)/$(MAIN_TARGET)

ifneq ($(SECOND_TARGET),)
TARGETS += $(ARCH)/$(SECOND_TARGET)
endif

ifneq ($(THIRD_TARGET),)
TARGETS += $(ARCH)/$(THIRD_TARGET)
endif

ifneq ($(FOURTH_TARGET),)
TARGETS += $(ARCH)/$(FOURTH_TARGET)
endif

all: $(TARGETS)
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
ifeq ($(MAIN_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc 1>/dev/null 2>1
else ifeq ($(MAIN_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
else
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
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
ifeq ($(SECOND_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc 1>/dev/null 2>1
else ifeq ($(SECOND_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
else
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
endif
endif

ifneq ($(THIRD_TARGET),)
$(ARCH)/$(THIRD_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)/$(THIRD_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ ! -d build ]; then exit 1; fi
	$(NOECHO)rm -rf build/binutils-2.31.1/build build/gcc-8.2.0/build
	$(NOECHO)echo Compiling Binutils \($(THIRD_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.31.1/build && cd build/binutils-2.31.1/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --with-sysroot --disable-nls --disable-werror 1>/dev/null 2>1 && make -j$(BUILD_CORES) 1>/dev/null 2>1 && make install -j$(BUILD_CORES) 1>/dev/null 2>1
	$(NOECHO)echo Compiling GCC \($(THIRD_TARGET)\)
ifeq ($(THIRD_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc 1>/dev/null 2>1
else ifeq ($(THIRD_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
else
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
endif
endif

ifneq ($(FOURTH_TARGET),)
$(ARCH)/$(FOURTH_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)/$(FOURTH_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ ! -d build ]; then exit 1; fi
	$(NOECHO)rm -rf build/binutils-2.31.1/build build/gcc-8.2.0/build
	$(NOECHO)echo Compiling Binutils \($(FOURTH_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.31.1/build && cd build/binutils-2.31.1/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --with-sysroot --disable-nls --disable-werror 1>/dev/null 2>1 && make -j$(BUILD_CORES) 1>/dev/null 2>1 && make install -j$(BUILD_CORES) 1>/dev/null 2>1
	$(NOECHO)echo Compiling GCC \($(FOURTH_TARGET)\)
ifeq ($(FOURTH_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc 1>/dev/null 2>1
else ifeq ($(FOURTH_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
else
	$(NOECHO)mkdir build/gcc-8.2.0/build && cd build/gcc-8.2.0/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH) --enable-languages=c,c++ --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
endif
endif
