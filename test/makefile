# File author is Ítalo Lima Marconato Matias
#
# Created on October 21 of 2018, at 11:43 BRT
# Last edited on October 21 of 2018, at 11:43 BRT

VERBOSE ?= false
OBJECTS := build/test.bliss

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(OBJECTS)

clean:
	$(NOECHO)rm -f $(OBJECTS)

clean-all:
	$(NOECHO)rm -rf build

remake: clean all

build/%.bliss: %.blasm
	$(NOECHO)echo Compiling $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)mono ../tools/blasm/build/blasm.exe -o $@ $<
