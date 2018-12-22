# File author is Ítalo Lima Marconato Matias
#
# Created on December 19 of 2018, at 23:51 BRT
# Last edited on December 20 of 2018, at 22:33 BRT

BOOTSECTS := cdboot

BOOTSECTS := $(addprefix build/$(SUBARCH)/,$(BOOTSECTS))

subarch: $(BOOTSECTS)

subarch-clean:
	$(NOECHO)rm -f $(BOOTSECTS)

build/$(SUBARCH)/%: subarch/$(SUBARCH)/bootsect/%.s
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-as $< -o $@.o
	$(NOECHO)$(TARGET)-ld -Tsubarch/$(SUBARCH)/bootsect/link.ld -o $@ $@.o
	$(NOECHO)rm $@.o
