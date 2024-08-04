OUTPUT	= bin
KERNEL	= $(OUTPUT)/kernel.sys

IMAGE	= $(OUTPUT)/risk-x86_64.hdd
IMGDIR	= $(OUTPUT)/image
IMGTMP 	= fat.img
FATTEN	= fatten.sh

LIMINE-LINK		= https://github.com/limine-bootloader/limine/raw/v7.x-binary/BOOTX64.EFI
LIMINE-FILE		= limine.efi

.PHONY			= clean rmtmpbin run debug debug-screen image kernel

$(IMAGE): rmtmpbin kernel
	@ echo "generating system image..."
## 	download the latest Limine EFI file
	@ echo "	downloading bootloader image..."
	@ wget $(LIMINE-LINK) -O $(LIMINE-FILE) --quiet
## 	organize the structure of the image
	@ echo "	organizing file contents..."
	@ mkdir $(IMGDIR)/efi/boot -p
	@ mkdir $(IMGDIR)/sys -p
	@ mkdir $(IMGDIR)/sys/start -p
	@ mkdir $(IMGDIR)/sys/share -p
	@ cp $(LIMINE-FILE) $(IMGDIR)/efi/boot/BOOTX64.EFI
	@ cp $(KERNEL) $(IMGDIR)/sys/start/kernel.sys
	@ cp limine.cfg $(IMGDIR)/limine.cfg
## 	compile the file contents into an image
	@ echo "	formatting final image..."
	@ dd if=/dev/zero of="$(IMGTMP)" bs=1M count="1024"
	@ mformat -i "$(IMGTMP)" -F -v "FAT32_IMAGE" ::
	@ mcopy -i "$(IMGTMP)" -s -v "$(IMGDIR)"/* ::/ > /dev/null 2>&1
## 	clean up and finalize
	@ echo "	cleaning up..."
	@ rm -rfd $(IMGDIR)
	@ mv $(IMGTMP) $(IMAGE)
	@ rm -f $(LIMINE-FILE)
	@ echo "done."

# only removes copied binaries and image
# done to avoid recompiling everything
# done to force for dependencies to be checked
rmtmpbin:
	@ rm -rf $(OUTPUT)

KERNEL_DIR	= kernel
$(KERNEL): rmtmpbin
	@ make -C $(KERNEL_DIR) -j12 all
	@ mkdir -p $(OUTPUT)
	@ cp $(KERNEL_DIR)/$(KERNEL) $(KERNEL)

kernel: $(KERNEL)
image: $(IMAGE)

clean:
	@ rm -rf $(OUTPUT)
	@ make -C $(KERNEL_DIR) clean

QEMU_MEM	= 1G
QEMU_ARGS = -bios OVMF.fd \
-drive file=$(IMAGE),format=raw \
-net none \
-m $(QEMU_MEM) \
-machine q35

run:
	@ qemu-system-x86_64 $(QEMU_ARGS) $(ADD_QEMU_ARGS)

debug:
	@ qemu-system-x86_64 $(QEMU_ARGS) -S -gdb tcp::1234 -d int -no-shutdown -no-reboot $(ADD_QEMU_ARGS)

debug-screen:
	@ screen -S qemu -d -m qemu-system-x86_64 $(QEMU_ARGS) -S -gdb tcp::1234 -d int -no-shutdown -no-reboot $(ADD_QEMU_ARGS)
	@ echo "Created QEMU instance in detached screen."
