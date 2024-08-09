section .text 
GLOBAL idt_reload
idt_reload:
    cli 
    lidt [rdi]
    sti ; might be an issue with APs later. Idk why it would, but maybe
    ret

