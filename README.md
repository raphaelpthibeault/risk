# risk OS
A hobby x86_64 multiprocessor operating system with plans to support RISCV64 and ARM64.

Is currently under development, and incomplete.

The OS includes a kernel and a POSIX-inspired C standard library, with plans to completely remove all dependencies save for qemu, clang and llvm i.e. a bootloader, dynamic shared object linker, etc.

## Dependencies
* nasm
* qemu
* grub, grub-mkrescue, grub-pc-bin
* xorriso
* mtools
* clang and llvm (version 18)

## Compiling
Requires a psf font (v1 or v2) in the fonts folder, with the name default.psf. Then, run:
```bash
make
```
which defaults to `build` and produces a bootable iso in the `build/` folder named risk-{ARCH}-{VERSION}.iso, e.g. `risk-x86_64-0.0.0.iso`.

## Run and Debug
To run with the qemu emulator:
```bash
make run 
```

To run in debug mode:
```bash
make debug
```
which will add the `-s -S` flags to the run command and open a remote target for gdb at `localhost:1234`. After running the command, with another shell launch `gdb build/kernel.bin` and use `target remote localhost:1234` to debug the qemu instance.

## Features
TODO

