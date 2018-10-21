# File author is Ítalo Lima Marconato Matias
#
# Created on October 21 of 2018, at 00:09 BRT
# Last edited on October 21 of 2018, at 00:13 BRT

VERBOSE ?= false
OUTPUT := build/blasm.exe

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(OUTPUT)

clean:
	$(NOECHO)rm -f $(OUTPUT)

clean-all:
	$(NOECHO)rm -rf build

remake: clean all

$(OUTPUT):
	$(NOECHO)echo Linking $@
	$(NOECHO)msbuild /nologo /noconlog
	$(NOECHO)rm -rf obj
