include Make.defaults

TARGET := $(BINDIR)/bootx64.efi

SRC += src/libc/string.c

CFLAGS += $(CFLAGS_DISABLE_SIMD) -fpic -fno-stack-check \
	-fshort-wchar -mno-red-zone -Wno-array-bounds \
	-D__BOOTLOADER__ -D__EMBED__ \
	-Iinclude/bootloader -Ideps/gnu-efi/inc

all: $(TARGET)

.PHONY: all

include Make.rules

