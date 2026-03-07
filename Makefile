OBJECTS = loader.o kmain.o src/io.o src/gdt.o src/gdts.o src/idts.o src/idt.o src/pic.o src/drivers/keyboard.o

CC = i686-elf-gcc

CFLAGS = -ffreestanding -Wall -Wextra -Werror -c -g

LDFLAGS = -T link.ld -nostdlib

AS = nasm
ASFLAGS = -f elf

all: kernel.elf

kernel.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o kernel.elf -lgcc

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R                              \
				-b boot/grub/stage2_eltorito    \
				-no-emul-boot                   \
				-boot-load-size 4               \
				-A os                           \
				-input-charset utf8             \
				-quiet                          \
				-boot-info-table                \
				-o os.iso                       \
				iso

run: os.iso
	qemu-system-i386 -boot d -cdrom os.iso -m 512 -no-reboot -s

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o kernel.elf os.iso iso/boot/kernel.elf src/*.o src/drivers/*.o