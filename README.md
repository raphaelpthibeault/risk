## Dependencies

You will need to install the RISC-V toolchain, instructions can be found at https://github.com/riscv-collab/riscv-gnu-toolchain

You will also need QEMU for RISC-V architectures, instructions at https://www.qemu.org/docs/master/system/riscv/virt.html

# Compiling bootloader
Compilation step: `riscv64-unknown-linux-gnu-as boot.S -o boot.o`

Linking step: `riscv64-unknown-linux-gnu-ld -T kernel.lds boot.o -o kernel.elf`

Run: `qemu-system-riscv64 -machine virt -cpu rv64 -smp 4 -m 128M -nographic -serial mon:stdio -bios none -kernel kernel.elf`


