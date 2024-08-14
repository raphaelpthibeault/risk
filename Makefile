include Make.defaults
include Make.rules

VERSION := 0.0.0
IMAGE_BASE_NAME ?= risk
ARCH = x86_64
ISO_IMAGE_FILENAME := $(IMAGE_BASE_NAME)-$(ARCH)-$(VERSION).iso

# rules
CFILES := $(shell find $(PROJECT_FOLDER) -type f -name "*.c")
ASFILES := $(shell find $(PROJECT_FOLDER) -type f -name "*.s")
FONTFILES := $(shell find $(FONT_FOLDER) -type f -name "*.psf")

ASOBJFILES := $(patsubst src/%.s, $(BUILD_FOLDER)/%.s.o, $(ASFILES))
COBJFILES := $(patsubst src/%.c, $(BUILD_FOLDER)/%.c.o, $(CFILES))
FONTOBJFILES := $(patsubst $(FONT_FOLDER)/%.psf, $(BUILD_FOLDER)/%.psf.o, $(FONTFILES))

HEADER_DEPS := $(patsubst $(PROJECT_FOLDER)/%, $(BUILD_FOLDER)/%, $(CFILES:.c=.c.d) $(ASFILES:.s=.s.d))

OBJ := $(COBJFILES) $(ASOBJFILES) $(FONTOBJFILES)

default: build

.PHONY: default build run clean compile_commands

build: $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME)

run: build 
	qemu-system-x86_64 -M q35 -m 2G -cdrom $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME) -boot d -smp 4 -serial mon:stdio

clean:
	rm -rf $(BUILD_FOLDER)
	find -name *.o -type f -delete

compile_commands:
	bear -- make build

# need iso_image_filename, keep here
$(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME): $(BUILD_FOLDER)/kernel.bin grub.cfg
	mkdir -p $(BUILD_FOLDER)/isofiles/boot/grub
	cp grub.cfg $(BUILD_FOLDER)/isofiles/boot/grub
	cp $(BUILD_FOLDER)/kernel.bin $(BUILD_FOLDER)/isofiles/boot
	cp $(BUILD_FOLDER)/kernel.map $(BUILD_FOLDER)/isofiles/boot
	grub-mkrescue -o $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME) $(BUILD_FOLDER)/isofiles

$(BUILD_FOLDER)/kernel.bin: $(OBJ) src/linker.ld
	echo $(OBJ)
	$(LD) -n -o $(BUILD_FOLDER)/kernel.bin -T src/linker.ld $(OBJ) -Map $(BUILD_FOLDER)/kernel.map

-include $(HEADER_DEPS)

$(BUILD_FOLDER)/%.c.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
	$(CC) ${CFLAGS} -c "$<" -o "$@"

$(BUILD_FOLDER)/%.s.o: src/%.s
	echo "$(<D)"
	mkdir -p "$(@D)"
	${AS} ${ASFLAGS} "$<" -o "$@"

$(BUILD_FOLDER)/%.psf.o: $(FONT_FOLDER)/%.psf
	echo "PSF: $(@D)"
	mkdir -p "$(@D)"
	objcopy -O elf64-x86-64 -B i386 -I binary "$<" "$@"


# gdb

