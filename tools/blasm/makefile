# File author is Ítalo Lima Marconato Matias
#
# Created on October 21 of 2018, at 00:09 BRT
# Last edited on October 21 of 2018, at 11:56 BRT

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

$(OUTPUT): Nodes/*.cs Properties/*.cs blasm.csproj *.cs
	$(NOECHO)echo Linking $@
	$(NOECHO)msbuild /nologo /noconlog
	$(NOECHO)rm -rf obj
