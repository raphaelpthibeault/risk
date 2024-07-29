#/bin/sh

riscv64-unknown-linux-gnu-as boot.S -o boot.o
riscv64-unknown-linux-gnu-ld -T kernel.lds boot.o -o kernel.elf
qemu-system-riscv64 -machine virt -cpu rv64 -smp 4 -m 128M -nographic -serial mon:stdio -bios none -kernel kernel.elf
