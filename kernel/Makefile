SOURCE	= src
OUTPUT	= bin

CCODE 	= $(shell find $(SOURCE) -type f -name '*.c')
SCODE	= $(shell find $(SOURCE) -type f -name '*.s')
OBJECTS	= $(patsubst $(SOURCE)/%.c, $(OUTPUT)/%.c.o, $(CCODE))
OBJECTS += $(patsubst $(SOURCE)/%.s, $(OUTPUT)/%.s.o, $(SCODE))

LDS = linker.ld

LINKER = $(OUTPUT)/kernel.sys

CC = clang-18
#LD = ld.lld
LD = clang-18
AS = nasm

SYMGEN	= ./symbol_gen.sh
SYMS	= __symbols.c

CCFLAGS = \
	-Og \
	-std=c23 \
	-ffreestanding \
	-I$(SOURCE) \
	-mno-red-zone \
	-mcmodel=kernel \
	-fno-stack-protector \
	-fpic \
	-fpie \
	-gdwarf \
	-fwrapv \
	-Werror \
	-pipe

LDFLAGS = \
	-nostdlib \
	-Wl,-T $(LDS) \
	-mcmodel=kernel \
	-pie

ASFLAGS = \
	-f elf64 \
	-g \
	-F dwarf

.PHONY			= clean all

$(OUTPUT)/%.c.o: $(SOURCE)/%.c
	@ echo "	compile $^"
	@ mkdir -p $(@D)
	@ $(CC) $(CCFLAGS) -c $^ -o $@

$(OUTPUT)/%.s.o: $(SOURCE)/%.s
	@ echo "	assemble $^"
	@ mkdir -p $(@D)
	@ $(AS) $(ASFLAGS) $^ -o $@

$(LINKER): $(OBJECTS)
	@ echo "	link $@ (1/2)"
	@ $(LD) $(LDFLAGS) $(OBJECTS) -o $(LINKER)
	@ echo "	generate symbols..."
	@ chmod 777 $(SYMGEN)
	@ $(SYMGEN) $(LINKER)
	@ echo "	compile symbols..."
	@ $(CC) $(CCFLAGS) -c $(SYMS) -o $(OUTPUT)/$(SYMS:.c=.o)
	@ echo "	link $@ (2/2)"
	@ $(LD) $(LDFLAGS) $(OBJECTS) $(OUTPUT)/$(SYMS:.c=.o) -o $(LINKER)
	@ rm $(SYMS)

clean:
	@ rm -rf $(OUTPUT)

all: $(LINKER)
