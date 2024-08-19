ENTRY(start)

SECTIONS {
    . = 1M;		/* skip a meg to avoid conflict with real mode */
    phys = .;	/* physical start */

    .text : ALIGN(4K)
	{
		*(.multiboot)
		*(.bootstrap)
		code = .;
		*(.text)
		*(.smp_ap_bootstrap) /* for the future */
	}

	.rodata : ALIGN(4K)
	{
		*(.rodata)
	}
	.rela.dyn : ALIGN(4K)
	{
		_rela_start = .;
		*(.rela)
		*(.rela.text)
		*(.rela.got)
		*(.rela.plt)
		*(.rela.bss)
		*(.rela.ifunc)
		*(.rela.text.*)
		*(.rela.data)
		*(.rela.data.*)
		*(.rela.rodata)
		*(.rela.rodata*)
		*(.rela.dyn)
		_rela_end = .;
	}

	.data : ALIGN(4K)
	{
		data = .;
		*(.data)
		*(.symbols)
		PROVIDE(kernel_symbols_start = .);
		PROVIDE(kernel_symbols_end = .);
	}

	.bss : ALIGN(4K)
	{
		PROVIDE(bss_start = .);
		bss = .;
		*(COMMON)
		*(.bss)
		/**(.stack)*/
	}

	end = .;

	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}

}

