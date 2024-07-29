.option norvc

.section .data 
welcome: .ascii "Welcome to risk\n\0"

.section .text.init 
.global _start 

_start: 
    csrr t0, mhartid    # get hardware thread 0
    bnez t0, _wait      # non-zero, get out
    call _setup_uart
    la a0, welcome
    call _write_uart
    wfi

_setup_uart:
    # disable all interrupts
    li t1, 0x10000001 
    sb x0, 0(t1)         
    # enable DLAB
    li t1, 0x10000003
    li t2, 0x80
    sb t2, 0(t1)         
    # set divisors (set baud rate divisor)
    li t1, 0x10000000
    li t2, 0x03         # to 3 (lo byte) 38400 baud
    sb t2, 0(t1)         
    li t1, 0x10000001
    li t2, 0x00         # to 0 (hi byte)
    sb t2, 0(t1)         
    # set output to: 8 bits, no parity, one stop bit
    li t1, 0x10000003   
    li t2, 0x03         
    sb t2, 0(t1)         
    # enable FIFO, clear them, with 14-byte threshold
    li t1, 0x10000002 
    li t2, 0xC7   
    sb t2, 0(t1)         
    # IRQs enabled RTS/DSR set
    li t1, 0x10000004 
    li t2, 0x0B
    sb t2, 0(t1)         
    ret

_write_uart: 
    # WRITE TO CONSOLE
    li t1, 0x10000000   # UART0 addr
    lb t2, 0(a0)
    beqz t2, _write_uart_end  
    sb t2, 0(t1) 
    li t2, 1
    add a0, t2, a0 
    j _write_uart

_write_uart_end:
    ret

_wait:
    wfi
