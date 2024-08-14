ENTRY(start)

SECTIONS {
    . = 1M; /* avoid conflict with real mode */

    _kernel_start = .;
    _kernel_virtual_offset = 0xffffffff80000000;

    /* multiboot 2 */
    .multiboot_header : {
        *(.multiboot_header)
    }

    .multiboot.text : {
        *(.multiboot.text)
    }

    . += _kernel_virtual_offset; /* jump to virtual memory */

    /* add a symbol indicating start address of the kernel */ 
	.text ALIGN (4K) : AT (ADDR (.text) - _kernel_virtual_offset) {
		*(.text)
		*(.text.*)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - _kernel_virtual_offset) {
		*(.rodata)
		*(.rodata.*)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - _kernel_virtual_offset) {
		*(.data)
		*(.data.*)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - _kernel_virtual_offset) {
		*(.bss)
	}

    _kernel_end = .;
    _kernel_physical_end = . - _kernel_virtual_offset;
}

