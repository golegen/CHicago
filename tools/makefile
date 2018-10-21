# File author is Ítalo Lima Marconato Matias
#
# Created on October 21 of 2018, at 00:12 BRT
# Last edited on October 21 of 2018, at 00:12 BRT

VERBOSE ?= false
BLASM := blasm/build/blasm.exe

ifneq ($(VERBOSE),true)
NOECHO := @
endif

.PHONY: $(BLASM)

all: $(BLASM)

clean:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm clean

clean-all:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm clean-all
	$(NOECHO)rm -rf build

remake:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm remake

$(BLASM):
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm all
