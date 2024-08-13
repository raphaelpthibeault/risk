include Make.defaults

MODULES := $(basename $(notdir $(wildcard makefiles/*.mk)))
APPS := $(basename $(notdir $(wildcard makefiles/apps/*.mk)))
TARGET := bin/risk.img

setup: 
	$(MAKE) -C deps/gnu-efi

$(MODULES): setup
	$(MAKE) -f makefiles/$@.mk SRCDIR=src/$@ BUILDDIR=build/$@ BINDIR=bin/$@

#$(APPS): $(MODULES)
#	$(MAKE) -f makefiles/apps/$@.mk SRCDIR=src/apps/$@ BUILDDIR=build/apps/$@ BINDIR=bin/apps

deploy: $(MODULES)  #$(APPS)
	dd if=/dev/zero of=$(TARGET) bs=1M count=64
	mkfs.vfat -F 32 -n "RISK" $(TARGET)
	mlabel -i $(TARGET) ::risk
	mmd -i $(TARGET) ::/boot
	mmd -i $(TARGET) ::/bin
	mmd -i $(TARGET) ::/efi
	mmd -i $(TARGET) ::/efi/boot
	mmd -i $(TARGET) ::/usr
	mmd -i $(TARGET) ::/usr/bin
	mcopy -i $(TARGET) -s root/* ::
	mcopy -i $(TARGET) -s bin/bootloader/bootx64.efi ::/efi/boot
	mcopy -i $(TARGET) -s bin/kernel/kernel ::/boot

#mcopy -i $(TARGET) -s bin/apps/shell ::/bin
#mcopy -i $(TARGET) -s bin/apps/calc ::/usr/bin
#mcopy -i $(TARGET) -s bin/apps/terminal ::/usr/bin

clean: 
	rm -rf build
	rm -rf bin

distclean: clean
	$(MAKE) -C deps/gnu-efi clean

.PHONY: all # $(APPS)
all: setup $(MODULES)  deploy


compile_commands: clean
	bear -- make all

run:
	qemu-system-x86_64 \
		-M q35 \
		-display sdl \
		-m 2G \
		-drive file=$(TARGET),format=raw \
		-smp 4 \
		-serial mon:stdio \
		-drive if=pflash,format=raw,unit=0,file=deps/OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=deps/OVMFbin/OVMF_VARS-pure-efi.fd \
		-net none \
		-no-shutdown

run_debug:
	qemu-system-x86_64 \
		-M q35 \
		-display sdl \
		-m 2G \
		-drive file=$(TARGET),format=raw \
		-smp 4 \
		-serial mon:stdio \
		-drive if=pflash,format=raw,unit=0,file=deps/OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=deps/OVMFbin/OVMF_VARS-pure-efi.fd \
		-net none \
		-no-shutdown \
		-no-reboot \
		-d int

