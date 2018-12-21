# File author is Ítalo Lima Marconato Matias
#
# Created on December 20 of 2018, at 10:49 BRT
# Last edited on December 20 of 2018, at 11:22 BRT

arch-clean:
	$(NOECHO)rm -f build/chicago-$(ARCH)_$(SUBARCH).iso

finish:
	$(NOECHO)echo Generating build/chicago-$(ARCH)_$(SUBARCH).iso
	$(NOECHO)if [ ! -d build ]; then mkdir -p build; fi
	$(NOECHO)if [ -d build/iso ]; then rm -rf build/iso; fi
	$(NOECHO)mkdir -p build/iso/Boot
	$(NOECHO)echo '"Boot from CHicago Install CD"=BootDevice,chicago' >> build/iso/Boot/bootmgr.conf
ifeq ($(SUBARCH),efi)
	$(NOECHO)dd if=/dev/zero of=build/iso/Boot/bootmgr.img bs=1k count=1440 2>/dev/null
	$(NOECHO)mformat -i build/iso/Boot/bootmgr.img -f 1440 ::
	$(NOECHO)mmd -i build/iso/Boot/bootmgr.img ::/EFI
	$(NOECHO)mmd -i build/iso/Boot/bootmgr.img ::/EFI/BOOT
	$(NOECHO)mcopy -i build/iso/Boot/bootmgr.img arch/$(ARCH)/build/$(SUBARCH)/bootmgr ::/EFI/BOOT/BOOTIA32.EFI
	$(NOECHO)xorriso -as mkisofs -R -c Boot/boot.cat -e Boot/bootmgr.img -U -no-emul-boot -o build/chicago-$(ARCH)_$(SUBARCH).iso build/iso 2>/dev/null
	$(NOECHO)rm -rf build/iso
else
	$(NOECHO)cp arch/$(ARCH)/build/$(SUBARCH)/cdboot build/iso/Boot/bootsect.bin
	$(NOECHO)cp arch/$(ARCH)/build/$(SUBARCH)/bootmgr build/iso/Boot/bootmgr.bin
	$(NOECHO)xorriso -as mkisofs -R -c Boot/boot.cat -b Boot/bootsect.bin -U -no-emul-boot -o build/chicago-$(ARCH)_$(SUBARCH).iso build/iso 2>/dev/null
	$(NOECHO)rm -rf build/iso
endif
