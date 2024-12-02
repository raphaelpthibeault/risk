TARGET=i686-elf

CC = $(TARGET)-gcc
AS = $(TARGET)-as

BIN_FILE = risk.bin
ISO_FILE = risk.iso

boot.o: boot.s
	$(AS) boot.s -o boot.o

kernel.o: kernel.c 
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(BIN_FILE): linker.ld boot.o kernel.o
	$(CC) -T linker.ld -o $(BIN_FILE) -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

build: $(BIN_FILE) grub.cfg
	mkdir -p isodir/boot/grub
	cp $(BIN_FILE) isodir/boot/$(BIN_FILE)
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_FILE) isodir

run: build
	qemu-system-i386 -cdrom $(ISO_FILE) -monitor stdio

compile_commands:
	bear -- make build

# debug

clean:
	rm -rf *.o *.iso $(BIN_FILE) isodir/


