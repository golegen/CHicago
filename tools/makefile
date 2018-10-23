# File author is Ítalo Lima Marconato Matias
#
# Created on October 21 of 2018, at 00:12 BRT
# Last edited on October 22 of 2018, at 13:32 BRT

VERBOSE ?= false

ifneq ($(VERBOSE),true)
NOECHO := @
endif

.PHONY: blasm

all: blasm

clean:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm clean

clean-all:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm clean-all

remake:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm remake

blasm:
	$(NOECHO)VERBOSE=$(VERBOSE) make -C blasm all
