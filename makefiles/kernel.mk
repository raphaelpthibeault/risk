include Make.defaults

TARGET := $(BINDIR)/kernel

SRC += $(wildcard src/libc/*.c) $(wildcard src/libc/*.s) \
	$(wildcard src/libc/*/*.c) $(wildcard src/libc/*/*.s)

CFLAGS += $(CFLAGS_DISABLE_SIMD) -fno-pic -mcmodel=large \
	-fno-stack-check -mno-red-zone -Wno-array-bounds \
	-fno-stack-protector \
	-D__EMBED__

ASFLAGS += -D__EMBED__

LDFLAGS += -T$(SRCDIR)/linker.ld

all: $(TARGET)

.PHONY: all

include Make.rules

