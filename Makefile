C_SOURCES = $(wildcard *.c src/*.c src/drivers/*.c src/kernel/*.c)
ASM_SOURCES = $(wildcard *.s src/*.s src/drivers/*.s src/kernel/*.s)

OBJECTS = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)

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
	qemu-system-i386 -boot d -cdrom os.iso -m 512 -no-reboot -s -d int,cpu_reset -D qemu.log

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	find . -type f -name '*.o' -delete
	rm -rf kernel.elf os.iso iso/boot/kernel.elf