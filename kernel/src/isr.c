#include <isr.h>
#include <interrupt_handler.h>

void (*g_isr_handlers[ISR_MAX])(interrupt_stack_t *, size_t);

void 
isr_init() {
    /* internals */
    isr_register_handler(8, double_fault_handler);

    isr_register_handler(13, general_protection_fault_handler);
    isr_register_handler(14, paging_fault_handler);
    
    /* irqs */
    isr_register_handler(33, keyboard_handler);
}

void 
isr_register_handler(size_t num, void (*handler)(interrupt_stack_t *, size_t)) {
    g_isr_handlers[num] = handler;
}

bool 
isr_exists(size_t num) {
    return g_isr_handlers[num] != 0;
}

