Just a fun hobby project to try to create my own little OS.
Currently, IDT, GDT, PIC, and a simple VGA text mode driver are implemented. Interrupts from the keyboard are also implemented.

Simply run `build_toolchain.sh` to build the whole toolchain, including the cross-compiler.

Then you can run `make run` to build the kernel and run it in QEMU. You can also run `make clean` to clean the build artifacts.

### Roadmap
- [x] Bootloader
- [x] VGA Text Mode Driver
- [x] GDT (Global Descriptor Table)
- [x] IDT & PIC (Hardware Interrupts)
- [x] Keyboard Driver
- [x] Automated Cross-Compiler Toolchain
- [X] Basic Error Printing
- [X] The APIC (System timer & heartbeat)
- [ ] Memory Management (Physical RAM & Paging)
- [ ] Kernel Heap (`malloc` & `free`)
- [ ] Multitasking
- [ ] User Space (Ring 3) & System Calls
- [ ] Disk Driver (ATA)
- [ ] File System (FAT32/ext2)
- [ ] ELF Loader (Running actual programs!)
