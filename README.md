Just a fun hobby project to try to create my own little OS.
Currently, IDT, GDT, PIC, and a simple VGA text mode driver are implemented. Interrupts from the keyboard are also implemented.

Simply run `build_toolchain.sh` to build the whole toolchain, including the cross-compiler.

Then you can run `make run` to build the kernel and run it in QEMU. You can also run `make clean` to clean the build artifacts.