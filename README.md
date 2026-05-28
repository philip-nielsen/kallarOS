# KallarOS

A hobby x86 32-bit operating system built from scatch in C and assembly. The goal of this project is to get an even deeper understanding of operating system development, low-level programming and most importantly to have fun! :)

Currently, the kernel successfully boots into a fully paged virtual memory environment, complete with physical/virtual memory allocators, hardware interrupts (IDT/PIC), an APIC system timer, and a basic VGA text-mode driver.

![Screenshot of OS booting in QEMU](kallaros.png)

## Getting Started

### Prerequisites

If you're on a debian-based system, simply run `build_toolchain.sh` to build the whole toolchain, including the cross-compiler.

It runs on Binutils(2.41) and GCC(13.2.0)

### Building and Running

Once the toolchain is built, use the Makefile to compile the kernel and launch it in QEMU:

- `make` - Builds the kernel and creates a bootable ISO image.
- `make run` - Builds the kernel and runs it in QEMU.
- `make clean` - Cleans up build artifacts.
- `make debug` - Boots the OS paused, waiting for a GDB connection on port 1234

### Roadmap

- [x] Bootloader
- [x] VGA Text Mode Driver
- [x] GDT (Global Descriptor Table)
- [x] IDT & PIC (Hardware Interrupts)
- [x] Keyboard Driver
- [x] Automated Cross-Compiler Toolchain
- [x] Basic Error Printing
- [x] The APIC (System timer & heartbeat)
- [x] Memory Management (Physical RAM & Paging)
- [x] Kernel Heap (`kmalloc` & `kfree`)
- [ ] Multitasking (Threads & Scheduling) Work in Progress
- [ ] Disk Driver (ATA)
- [ ] File System (FAT32/ext2)
- [ ] User Space (Ring 3) & System Calls
- [ ] ELF Loader (Running actual programs!)
