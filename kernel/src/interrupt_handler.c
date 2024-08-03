#include <isr.h>
#include <interrupt_handler.h>
#include <log.h>

static inline void 
_general_handler(interrupt_error_stack_t *stack, size_t num, char *fault_type) {
	/* TODO 
	 * stack trace 
	 * better logging functions */
	
	log_("FAULT TYPE: %s\n", fault_type);
	log_("NUM: %llu\n", num);
	log_("ERROR CODE: %x\n", stack->error_code);
	log_("RIP: 0x%x\n", stack->rip);
	log_("RSP: 0x%x\n", stack->rsp);
	log_("RBP: 0x%x\n", stack->gen.rbp);
}

void 
double_fault_handler(interrupt_stack_t* stack, size_t num) {
	_general_handler((interrupt_error_stack_t*)stack, num, "DOUBLE FAULT");
	while(true);
}

void 
tss_fault_handler(interrupt_stack_t* stack, size_t num) {
	_general_handler((interrupt_error_stack_t*)stack, num, "INVALID TASK STATE SEGMENT");
	while(true);
}

void 
segment_fault_handler(interrupt_stack_t* stack, size_t num) {
	_general_handler((interrupt_error_stack_t*)stack, num, "SEGMENT NOT PRESENT EXCEPTION");
	while(true);
}

void 
general_protection_fault_handler(interrupt_stack_t* stack, size_t num) {
	_general_handler((interrupt_error_stack_t*)stack, num, "GENERAL PROTECTION FAULT");
	while(true);
}

void 
paging_fault_handler(interrupt_stack_t* stack, size_t num) {
	_general_handler((interrupt_error_stack_t*)stack, num, "PAGE FAULT");
	while(true);
}

// *** IRQs *** //

void 
keyboard_handler(interrupt_stack_t* stack, size_t num) {
	/* TODO */
	log_("KEYBOARD\n");
	log_("num: %llu\n", num);
	log_("RIP: 0x%x\n", stack->rip);
	log_("RSP: 0x%x\n", stack->rsp);
	log_("RBP: 0x%x\n", stack->gen.rbp);
}

